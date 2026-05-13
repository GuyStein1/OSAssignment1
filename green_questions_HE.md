# שאלות ירוקות — משימות 2 ו-3

תשובות לשאלות התיאורטיות מה-PDF של המטלה.
לא להגשה, אבל צפוי שתדעו להסביר אותן בשיחת ההגנה.

---

# שאלות משימה 2 (עמוד 8)

## ש1: במה שונה קריאה ל-system call מקריאה לפונקציה רגילה? איך זה עובד?

קריאה לפונקציה רגילה נשארת כולה ב-user space. המעבד מבצע קפיצה (`jal`/`jalr` ב-RISC-V) לכתובת אחרת באותו מרחב כתובות, ללא שינוי הרשאות וללא מעורבות של ה-kernel.

לעומת זאת, system call חוצה את הגבול בין user space ל-kernel. הזרימה היא:

1. קוד המשתמש קורא לפונקציית stub (למשל `memsize()`), שנוצרת אוטומטית מ-`usys.pl`.
2. ה-stub טוען את מספר ה-syscall לרגיסטר `a7` ומבצע את פקודת `ecall`.
3. הפקודה `ecall` מפעילה trap: המעבד עובר ל-supervisor mode וקופץ ל-trap handler.
4. הפונקציה `usertrap()` בקובץ `kernel/trap.c` מזהה שמדובר ב-syscall וקוראת ל-`syscall()` בקובץ `kernel/syscall.c`.
5. הפונקציה `syscall()` קוראת את `a7` מה-trapframe ומעבירה לפונקציית `sys_*` המתאימה דרך טבלת ה-syscall.
6. ה-handler רץ ב-kernel mode עם הרשאות מלאות.
7. ערך ההחזרה נכתב לתוך `trapframe->a0`.
8. ה-kernel חוזר ל-user space דרך `sret`, ומשחזר את ה-user mode. כעת הרגיסטר `a0` מכיל את ערך ההחזרה.

ההבדל המרכזי: קריאת פונקציה היא קפיצה באותה רמת הרשאות ובאותו מרחב כתובות. לעומת זאת, system call הוא נקודת כניסה מבוקרת ל-kernel, אותה אוכפת החומרה.

---

## ש2: איך פרמטרים מועברים לפונקציית ה-system call? ואיך ערך ההחזרה חוזר ל-user space?

**פרמטרים:** מועברים ברגיסטרים `a0`, `a1`, `a2`, ... (מוסכמת הקריאה הסטנדרטית של RISC-V). כאשר ה-trap מופעל, ה-kernel שומר את כל הרגיסטרים של המשתמש בתוך ה-`trapframe` של ה-process. לאחר מכן, ה-kernel קורא את הארגומנטים מה-trapframe באמצעות פונקציות עזר כמו `argint()` ו-`argaddr()` המוגדרות בקובץ `kernel/syscall.c`.

עבור `memsize` אין ארגומנטים. עבור syscall כמו `write(fd, buf, n)`, ה-kernel יקרא:
```c
argint(0, &fd);    // reads from trapframe->a0
argaddr(1, &buf);  // reads from trapframe->a1
argint(2, &n);     // reads from trapframe->a2
```

**ערך החזרה:** ה-handler מחזיר `uint64`. הפונקציה `syscall()` שומרת אותו ב-`trapframe->a0`. כאשר ה-kernel חוזר ל-user space, הרגיסטר `a0` מכיל את הערך הזה, אותו מוסכמת ה-C מפרשת כערך החזרה של הפונקציה.

---

## ש3: מה המטרה של קובץ `usys.pl`?

הקובץ `usys.pl` הוא סקריפט Perl שמייצר אוטומטית את הקובץ `user/usys.S` בזמן הבנייה. עבור כל קריאה `entry("name")` בסקריפט, הוא מייצר stub assembly קטן כזה:

```asm
memsize:
  li a7, SYS_memsize
  ecall
  ret
```

כלומר, אנחנו לא כותבים assembly ידנית עבור syscall stubs. הוספת syscall חדש דורשת רק שורה אחת ב-`usys.pl`. הקובץ `usys.S` שנוצר מקומפל ומקושר לכל תוכנית משתמש, כך שקריאה ל-`memsize()` מ-C מפעילה את ה-`ecall` הנכון באופן שקוף.

---

## ש4: מה זה `struct proc` ואיפה הוא מוגדר? למה אנחנו צריכים אותו? האם למערכות הפעלה אמיתיות יש מבנה דומה?

המבנה `struct proc` מוגדר בקובץ `kernel/proc.h` בשורה 85. זהו ה-**Process Control Block (PCB)** של xv6 — מבנה הנתונים שה-kernel מחזיק לכל process. הוא מכיל כל מה שה-kernel צריך כדי לנהל process:

```c
struct proc {
  struct spinlock lock;
  enum procstate state;        // UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE
  void *chan;                  // sleep channel
  int killed;
  int pid;
  struct proc *parent;
  uint64 sz;                   // process memory size in bytes (used by memsize())
  pagetable_t pagetable;       // user page table
  struct trapframe *trapframe; // saved user registers
  struct context context;      // saved kernel registers (for swtch)
  struct file *ofile[NOFILE];  // open file descriptors
  struct inode *cwd;           // current working directory
  char name[16];
};
```

ללא ה-PCB, ל-kernel אין דרך לשמור את מצב ה-process כשהוא לא רץ, לתזמן אותו, לשחזר אותו, או לעקוב אחרי המשאבים שלו. זוהי היחידה הבסיסית שה-scheduler פועל עליה.

לכל מערכת הפעלה אמיתית יש מבנה שקול. ב-Linux הוא נקרא `task_struct` (מוגדר בקובץ `include/linux/sched.h`) והוא הרבה יותר גדול (כ-700 שדות), אך משמש לאותה מטרה. ל-Windows יש `EPROCESS`. הרעיון אוניברסלי.

---

## ש5: כמה זיכרון התוכנית שלנו משתמשת לפני ואחרי ההקצאה?

```
memory before allocation: 16384 bytes
memory after allocation:  81920 bytes
```

ה-process מתחיל ב-16384 בייטים (הקוד + הנתונים + ה-stack הראשוניים שנטענו על ידי ה-OS). אחרי `malloc(20000)`, גודל ה-process קופץ ל-81920 בייטים — עלייה של 65536 בייטים, הרבה יותר מ-20000 שהתבקשו.

---

## ש6: מה ההבדל בגודל הזיכרון לפני ואחרי ה-free?

```
memory after allocation: 81920 bytes
memory after free:       81920 bytes
```

אין הבדל. הפונקציה `free()` לא מקטינה את `memsize()`.

---

## ש7: למה `free()` לא מקטין את גודל הזיכרון? (מתקדם: ראו את המימוש של `malloc` ו-`free`)

הפונקציה `free()` ב-xv6 ממומשת בקובץ `user/umalloc.c` כ-allocator קלאסי של K&R עם רשימת free. היא פועלת כולה ב-user space: היא מחברת את הבלוק המשוחרר בחזרה לתוך רשימה מקושרת (`freep`) כדי שקריאות עתידיות ל-`malloc` יוכלו לעשות בו שימוש חוזר. היא **לעולם לא** קוראת ל-`sbrk` עם ערך שלילי כדי להחזיר זיכרון ל-kernel.

הקריאה `memsize()` מחזירה את `p->sz` — גודל ה-process כפי שה-kernel עוקב אחריו. ה-kernel משנה את `sz` רק כאשר `sbrk` נקרא. מכיוון ש-`free()` לא קורא ל-`sbrk`, הערך `p->sz` נשאר 81920.

**למה `malloc(20000)` גרם לקפיצה של 65536 בייטים?**

כאשר `malloc` צריך זיכרון מה-kernel בפעם הראשונה, הוא קורא ל-`morecore()`. הפונקציה `morecore` תמיד מבקשת לפחות 4096 יחידות הקצאה (`if(nu < 4096) nu = 4096`), כדי להימנע מקריאה ל-`sbrk` בכל הקצאה קטנה. כל יחידה היא `sizeof(Header)` = 16 בייטים (union שמיושרת ל-`long` על 64-ביט). לכן:

```
4096 units × 16 bytes = 65536 bytes
16384 (before) + 65536 = 81920 bytes
```

זה תואם בדיוק לפלט שנצפה. הזיכרון הנוסף מעבר ל-20000 שהתבקשו נשאר ב-free list של ה-allocator, מוכן לקריאות עתידיות ל-`malloc` ללא צורך בעוד `sbrk`.

מודל מחשבה שימושי: הקריאה `sbrk` היא כמו רוצ'ט חד-כיווני — היא רק עולה. הפונקציות `malloc` ו-`free` מנהלות pool בתוך המרחב שכבר נתבע מה-kernel. אחרי `free`, ה-allocator מסמן את הבלוק כזמין שוב, כך שה-`malloc` הבא יוכל לעשות בו שימוש חוזר ללא קריאה ל-`sbrk`. ה-kernel רואה רק את קו המים הגבוה ביותר.

---

# שאלות משימה 3 (עמוד 15)

## ש1: מה ההבדל בין multitasking קואופרטיבי לבין preemptive? איך `co_yield` מממש multitasking קואופרטיבי?

ב-**preemptive multitasking**, ה-OS יכול להפסיק כפייתית process שרץ בכל רגע (בדרך כלל דרך timer interrupt) ולעבור לאחר. ל-process אין מה לומר לגבי מתי הוא מאבד את המעבד. כך עובד ה-scheduler הרגיל של xv6 — timer מופעל, מפעיל `usertrap`, וה-scheduler בוחר את ה-process הבא ב-RUNNABLE.

ב-**cooperative multitasking**, process רץ עד שהוא מוותר מרצונו על המעבד. שום process לא יכול להיות מופסק בניגוד לרצונו. המערכת עוברת רק כאשר process קורא לפעולה דמוית yield.

המימוש שלנו של `co_yield` הוא multitasking קואופרטיבי כי:
- process מוותר על המעבד רק כאשר הוא קורא במפורש ל-`co_yield`.
- ה-process המטרה נבחר על ידי ה-process הקורא (לפי PID), לא על ידי ה-scheduler.
- ה-kernel מבצע את המעבר מיד וישירות, מבלי לערב את ה-timer או את לולאת ה-scheduler.

---

## ש2: איך `co_yield` עוקף את ה-scheduler הרגיל? מה היתרונות והחסרונות?

**איך הוא עוקף את ה-scheduler:**

במקום לקרוא ל-`sched()` (שחוזר ללולאת ה-scheduler, שאז בוחרת את ה-process הבא ב-RUNNABLE), הקריאה ל-`co_yield` מפעילה ישירות:

```c
mycpu()->proc = target;
swtch(&p->context, &target->context);
```

הקוד הזה מחליף את מצביע ה-process הנוכחי של ה-CPU וקופץ ישירות להקשר ה-kernel של ה-process המטרה. לולאת ה-scheduler לא נכנסת לתמונה.

**יתרונות:**
- אפס תקורה של scheduling — אין לולאה בטבלת ה-processes, אין המתנה על locks של processes אחרים.
- דטרמיניסטי: יודעים בדיוק איזה process ירוץ אחר כך.
- זמן latency נמוך יותר לצמדי producer/consumer צמודים.

**חסרונות:**
- עובד נכון רק עם מעבד אחד (מרובי מעבדים דורש סנכרון מורכב הרבה יותר).
- שובר הנחות של ה-scheduler לגבי בעלות על locks (ומכאן התיקון ב-scheduler).
- לא גנרי: מטפל בצורה נקייה רק במקרה של העברה הדדית בין שני processes.
- process שלא מחזיר ל-`co_yield` יגרום לשותפו להיתקע לצמיתות.

---

## ש3: איך הערך השלם מועבר בין processes? איפה הוא מאוחסן ואיך הוא נשלף?

הערך עובר דרך רגיסטרי ה-trapframe, תוך שימוש חוזר בשדות שכבר קיימים לכל process:

1. כאשר process קורא ל-`co_yield(pid, value)`, ה-kernel שומר את `value` ב-`p->trapframe->a1`. זה עובד כי `a1` כבר הכיל את הארגומנט השני של ה-syscall כאשר ה-trap הופעל — אין צורך באחסון חדש.

2. כאשר ה-process השני מגיע ושני הצדדים מוכנים, ה-kernel מחליף בצלב:
```c
p->trapframe->a0 = target->trapframe->a1;   // we receive target's value
target->trapframe->a0 = value;              // target receives our value
```

3. אחרי ההמשך מ-`swtch`, כל process קורא את ערך ההחזרה שלו מ-`trapframe->a0`:
```c
ret = p->trapframe->a0;
return ret;
```

הרגיסטר `a0` הוא הרגיסטר שה-kernel משתמש בו עבור ערכי החזרה של syscall, כך שכתיבה ל-`trapframe->a0` היא בדיוק הדרך לקבוע מה יוחזר ל-user space.

---

## ש4: באילו תרחישים אמיתיים מנגנון yield בסגנון coroutine שימושי?

**תרחישים:**
- צינורות **producer/consumer**: parser שמספק tokens למחולל קוד. כל צד מוותר לשני כאשר ייצר או צרך יחידת עבודה.
- **מנועי משחקים**: לוגיקת המשחק והרינדור מחליפים שליטה באופן קואופרטיבי בכל frame ללא תקורת thread.
- **frameworks של async I/O**: Node.js, asyncio של Python, וgoroutines של Go משתמשים כולם ב-cooperative yielding כדי שמשימות תלויות I/O ימסרו את המעבד ל-event loop בזמן ההמתנה.
- **ספריות coroutine**: generators של Python (`yield`), coroutines של Lua, ו-coroutines של C++20 מממשים כולם את אותה תבנית rendezvous ברמת השפה.

השפה Go היא האנלוג הישיר ביותר: goroutines מתוזמנות בצורה קואופרטיבית (עם preemption שנוסף מאוחר יותר), ו-`runtime.Gosched()` הוא בעצם `co_yield` ל-runtime scheduler של Go.

---

## ש5: במה שונה מעבר ישיר בין processes ב-`co_yield` ממעבר context switch רגיל על ידי ה-scheduler?

| | מעבר scheduler רגיל | מעבר ישיר של `co_yield` |
|---|---|---|
| מי בוחר את המטרה? | לולאת ה-scheduler (round-robin על RUNNABLE) | ה-process הקורא (לפי PID) |
| הנתיב | `sched()` ← לולאת scheduler ← `swtch` להקשר ה-scheduler ← scheduler בוחר הבא ← `swtch` ל-process | `swtch` ישיר מה-process הנוכחי ל-process המטרה |
| הקשר ביניים | עובר דרך ה-stack של ה-scheduler לכל CPU | אין — קפיצה ישירה |
| מצב המטרה לפני המעבר | חייב להיות RUNNABLE | יכול להיות SLEEPING (כבר בתוך co_yield) או RUNNABLE |
| משמעת lock | הקורא מחזיק את ה-lock שלו בכניסה ל-`sched()`, ה-scheduler משחרר אותו | הקורא משחרר את ה-lock שלו, ה-lock של המטרה מוחזק דרך ה-`swtch` |

ההבדל המרכזי: מעבר רגיל עובר נוכחי ← scheduler ← הבא (שתי קפיצות). מעבר של `co_yield` עובר נוכחי ← מטרה (קפיצה אחת), ולכן ה-latency שלו נמוך יותר וזו הסיבה שהוא יוצר את בעיית בעלות ה-lock ב-scheduler המוסברת ב-README.

---

## ש6: מה היה קורה אם ניסינו לממש זאת רק עם `sleep` ו-`wakeup` הקיימים?

באמצעות `sleep`/`wakeup` פשוטים ניתן לשים process לישון ולהעיר את המטרה, אך היו חסרים שני דברים קריטיים:

**1. אין העברת ערך.** לפונקציות `sleep` ו-`wakeup` אין מנגנון למסירת payload. היה צריך אזור זיכרון משותף או syscall נוסף, וזה בעצם הוספת מבנה נתונים גלובלי — דבר שאסור על ידי המטלה.

**2. אין מסירת CPU ישירה.** הקריאה `sleep` מפעילה את `sched()`, שחוזר ללולאת ה-scheduler. ה-scheduler אז בוחר את ה-process ה-RUNNABLE הבא שהוא מוצא — אין ערובה שהמטרה תרוץ מיד. המטרה תועבר ל-RUNNABLE אבל עלולה לחכות מספר ticks של scheduler לפני שתרוץ בפועל. ה-`swtch` הישיר של `co_yield` מבטיח שהמטרה תרוץ הבא ללא תזמון ביניים.

כלומר, הזוג `sleep`/`wakeup` יכל לממש rendezvous רופף אך לא מסירת coroutine אמיתית עם החלפת ערכים ומסירת CPU ישירה מובטחת.

---

## ש7: איך `co_yield` משתלב עם מצבי ה-process של xv6?

המימוש של `co_yield` משתמש רק בארבעת המצבים הקיימים ועושה שימוש מחדש ב-`SLEEPING` עם channel מיוחד כדי להבחין בין שינה של coroutine לבין שינה רגילה:

- **RUNNING ← SLEEPING:** ה-process הקורא מעביר את עצמו ל-SLEEPING עם `chan = co_chan(target)` לפני שעובר הלאה.
- **SLEEPING ← RUNNING:** ה-process המטרה (שכבר ישן בתוך `co_yield`) מועבר ישירות ל-RUNNING על ידי ה-process המגיע. השדה `target->chan` מנוקה.
- **RUNNABLE ← RUNNING:** אם המטרה טרם קראה ל-`co_yield`, היא ב-RUNNABLE. הקורא הראשון מעביר אותה ישירות ל-RUNNING, ומדלג על ה-RUNNABLE כנדרש ב-PDF.

ערך ה-channel המיוחד `&p->context` הוא מה שמבחין בין process שישן ב-coroutine לבין process שישן על pipe, lock, או המתנה לדיסק. ה-scheduler מתעלם מ-processes במצב SLEEPING מעצם תכנונו, כך ש-processes ישנים של coroutine לא ייבחרו בטעות — ניתן להעיר אותם רק על ידי ה-peer שלהם ב-co_yield.

---

## ש8: מה ההשלכות על ביצועים של עקיפת ה-scheduler?

**מועיל כאשר:**
- שני ה-processes צמודים ותמיד מוכנים להחליף מיד. כל מסירה היא קריאה אחת ל-`swtch` במקום שתיים (נוכחי ← scheduler ← הבא).
- זמן latency חשוב יותר מהוגנות — למשל, pipelines רגישי latency או real-time.
- יש הרבה processes במצב idle: ה-scheduler יבזבז זמן בסריקת טבלת ה-processes בחיפוש אחר RUNNABLE; הקריאה `co_yield` קופצת ישירות לנכון.

**מזיק כאשר:**
- המטרה לא מוכנה (למשל, מחכה ל-I/O). עם מסירה ישירה, המטרה חוזרת מיד מ-`co_yield` עם `-1` או שגיאה אחרת, בעוד ה-scheduler היה מוצא process פרודוקטיבי להריץ במקום.
- הוגנות נדרשת. עקיפת ה-scheduler אומרת ש-processes אחרים לא מקבלים זמן CPU במהלך החלפת ה-coroutine. על מערכת עמוסה, שני processes שמשחקים פינג-פונג דרך `co_yield` יכולים לגרום ל-starvation לכל השאר.
- מרובי מעבדים מעורבים. מעבר ישיר ללא סנכרון בין מעבדים גורר מצבי race — ולכן המטלה מגבילה את `co_yield` ל-`CPUS := 1`.
