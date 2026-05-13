# הערות לימוד — מטלה 1 במערכות הפעלה

קובץ זה מסביר מה שינינו במטלה 1, מדוע כל שינוי קיים,
ומה עלינו להיות מסוגלים להסביר בשיחת ההגנה.

המימוש הסופי ממוזג ל-`main`. ה-branch בשם `stein` מכיל את
אותו קוד מטלה סופי.

קבצי מקור ללימוד יחד עם README זה:

- הקובץ `os262-assignment1.pdf`
  - הוראות המטלה הרשמיות.
  - מגדיר את משימות 1, 2 ו-3.
  - נותן את פורמט `co_test` הנדרש למשימה 3.

- הקובץ `moodle_qa.md`
  - תשובות והבהרות של צוות הקורס.
  - חשוב להבנת מה הצוות רואה כמקובל.

- ה-README הנוכחי
  - הערות המימוש שלנו.
  - ההנמקה מאחורי שינויי הקוד.
  - שאלות שעלינו להיות מוכנים לענות בעל-פה.

## סטטוס מהיר

משימות שמומשו:

- משימה 0: xv6 בונה ומאתחל ב-devcontainer.
- משימה 1: תוכנית משתמש `helloworld`.
- משימה 2: system call בשם `memsize()` ובדיקה `memsize_test`.
- משימה 3: system call בשם `co_yield(pid, value)` ובדיקה `co_test`.
- שלושת בדיקות השגיאה הנדרשות של משימה 3 רצות בתחילת `co_test`
  (לפני לולאת הפינג-פונג של ה-PDF), כך שאין תוכנית `co_error_test` נפרדת בגרסה הסופית.

פקודות חשובות:

```sh
make qemu
```

בתוך xv6:

```sh
helloworld
memsize_test
co_test
```

יציאה מ-QEMU:

```text
Ctrl-a
x
```

לפני הגשה סופית:

```sh
make clean
```

המטלה מחייבת הגשת עץ מקור מלא של xv6, לא רק הקבצים שהשתנו.

## מאגר ותהליך עבודה

התחלנו ממאגר הקורס:

```text
https://github.com/BGU-CS-OS/xv6-riscv
```

לאחר מכן יצרנו מאגר GitHub משלנו:

```text
https://github.com/GuyStein1/OSAssignment1
```

ה-remotes הם:

```text
origin   -> our repository
upstream -> the course repository
```

מדוע זה חשוב:

- ה-remote בשם `origin` הוא המקום שאנחנו דוחפים אליו את העבודה שלנו.
- ה-remote בשם `upstream` הוא רק מאגר המקור שממנו התחלנו.
- אנחנו לא אמורים לדחוף עבודת מטלה למאגר הקורס.

שמרנו על `main` כ-branch בסיסי משותף ומימשנו על `stein`:

```sh
git switch main
git pull --ff-only origin main
git switch -c stein
```

מדוע השתמשנו ב-branch נפרד:

- שמירה על `main` נקי בזמן שאנחנו מתנסים.
- השותף יכול לעבוד על branch אחר.
- מאוחר יותר אפשר להשוות branches לפני בחירת הגרסה הסופית.

ה-commits המרכזיים של המטלה הם:

```text
Add xv6 hello world user program
Add memsize syscall and test program
Add co_yield coroutine syscall
```

פקודות Git שימושיות:

```sh
git status
git log --oneline --decorate --max-count=8
git diff stein..origin/<teammate-branch>
git push -u origin stein
```

## סביבת פיתוח

השתמשנו ב-VS Code Dev Containers עם Rancher Desktop כמנוע Docker.
הדבר עובד כי Rancher Desktop מספק runtime תואם Docker.

קבצי devcontainer הם:

- הקובץ `.devcontainer/devcontainer.json`
- הקובץ `.devcontainer/Dockerfile`

ה-Dockerfile מתקין את הכלים ש-xv6 צריך:

- חבילת `gcc-riscv64-linux-gnu`
- חבילת `binutils-riscv64-linux-gnu`
- חבילת `qemu-system-misc`
- חבילת `gdb-multiarch`
- חבילת `build-essential`
- חבילת `git`

מדוע אנחנו משתמשים ב-container:

- מערכת xv6 צריכה cross compiler של RISC-V.
- מערכת xv6 רצה תחת QEMU, לא ישירות על macOS.
- ה-container נותן לכולם את אותה הגדרת compiler ו-emulator.

בתוך ה-container, נתיב המאגר הוא בדרך כלל:

```text
/workspaces/xv6-riscv
```

במחשב המארח, אותו מאגר נמצא ב:

```text
/Users/guyst/CS/LastSemester!/OperatingSystems/xv6-riscv
```

פרט חשוב לגבי הטרמינל:

- הפקודה `make qemu` רצה על ה-Linux shell בתוך ה-container.
- אחרי ש-xv6 מאתחל, ה-prompt משתנה ל-shell של xv6: `$`.
- פקודות כמו `helloworld`, `memsize_test` ו-`co_test` רצות בתוך xv6.
- פקודות host/container כמו `make clean` לא עובדות בתוך xv6.

לכן זה קרה:

```text
$ make clean
exec make failed
```

זו לא הייתה בעיית build. זה אומר רק ש-`make clean` הוקלד בתוך xv6
במקום ב-shell של ה-container.

קודם יוצאים מ-xv6/QEMU:

```text
Ctrl-a
x
```

לאחר מכן מריצים פקודות host/container כמו:

```sh
make clean
```

## קבצים שינינו

קבצים מרכזיים:

- הקובץ `Makefile`
  - מוסיף את תוכניות המשתמש שלנו ל-`UPROGS`.
  - מגדיר `CPUS := 1`, כנדרש למשימה 3.

- הקובץ `user/helloworld.c`
  - תוכנית משתמש של משימה 1.

- הקובץ `kernel/syscall.h`
  - מוסיף מספרי syscall עבור `memsize` ו-`co_yield`.

- הקובץ `kernel/syscall.c`
  - רושם kernel syscall handlers בטבלת ה-syscall.

- הקובץ `kernel/sysproc.c`
  - מממש wrappers של syscall:
    - `sys_memsize`
    - `sys_co_yield`

- הקובץ `kernel/defs.h`
  - חושף את פונקציית ה-kernel `co_yield`.

- הקובץ `kernel/proc.c`
  - מממש את לוגיקת מסירת ה-coroutine האמיתית.
  - מתאים את ה-scheduler לטיפול במעבר ישיר בין processes.

- הקובץ `user/usys.pl`
  - מוסיף syscall stubs של user-space שנוצרים אוטומטית.

- הקובץ `user/user.h`
  - מוסיף הצהרות של user-space.

- הקובץ `user/memsize_test.c`
  - תוכנית בדיקה של משימה 2.

- הקובץ `user/co_test.c`
  - תוכנית הבדיקה הנדרשת של משימה 3 מה-PDF.
  - מריצה תחילה את שלושת מקרי השגיאה הנדרשים, לאחר מכן נכנסת
    ללולאת מסירת הורה/ילד האינסופית של ה-PDF.

## איך System Calls עובדים ב-xv6

הנתיב החשוב עבור system call הוא:

```text
user C code
  -> generated assembly stub from user/usys.pl
  -> ecall
  -> usertrap() in kernel/trap.c
  -> syscall() in kernel/syscall.c
  -> sys_* handler
  -> return value placed in trapframe->a0
  -> back to user space
```

ה-process של המשתמש מעביר את מספר ה-syscall ברגיסטר `a7`. הארגומנטים
מועברים ברגיסטרים `a0`, `a1`, וכן הלאה. מערכת xv6 שומרת את הרגיסטרים האלה ב-`trapframe`
של ה-process הנוכחי.

לכן הוספת syscall תמיד דורשת מספר עריכות:

1. מספר syscall ב-`kernel/syscall.h`.
2. הצהרת kernel handler ורשומה בטבלה ב-`kernel/syscall.c`.
3. פונקציית `sys_*` בקובץ kernel, כאן `kernel/sysproc.c`.
4. הוספת stub של user-space ב-`user/usys.pl`.
5. הצהרה של user-space ב-`user/user.h`.

הקובץ `user/usys.S` נוצר על ידי `user/usys.pl`. אנחנו לא עורכים אותו ידנית.

## משימה 1: `helloworld`

מטרת המשימה: ליצור תוכנית xv6 של user-space שמדפיסה:

```text
Hello World xv6
```

מה עשינו:

- הוספנו את הקובץ `user/helloworld.c`.
- הוספנו את `$U/_helloworld` ל-`UPROGS` ב-`Makefile`.

מדוע `_helloworld` ב-Makefile:

- תוכניות משתמש ב-xv6 מקומפלות לקבצים שהשמות שלהם מתחילים ב-`_`.
- תמונת מערכת הקבצים של xv6 כוללת את הקובץ `_program`.
- ה-shell מריץ אותו כ-`program`, ללא קו תחתי.

כלומר:

```text
user/helloworld.c -> user/_helloworld -> runs as helloworld in xv6
```

פלט צפוי:

```text
$ helloworld
Hello World xv6
```

מה לדעת לציון:

- זוהי תוכנית user-space, לא קוד macOS/Linux של המארח.
- היא רצה בתוך xv6 תחת QEMU.
- היא משתמשת ב-`printf` ו-`exit` של xv6 עצמו, המוצהרים ב-`user/user.h`.

## משימה 2: `memsize()`

מטרת המשימה: להוסיף:

```c
int memsize(void);
```

הפונקציה מחזירה את גודל הזיכרון של ה-process הנוכחי בבייטים.

מה שינינו:

- בקובץ `kernel/syscall.h`
  - הוספנו `SYS_memsize 22`.

- בקובץ `kernel/syscall.c`
  - הוספנו `extern uint64 sys_memsize(void);`.
  - הוספנו `[SYS_memsize] sys_memsize` לטבלת ה-syscall.

- בקובץ `kernel/sysproc.c`
  - הוספנו:

```c
uint64
sys_memsize(void)
{
  return myproc()->sz;
}
```

- בקובץ `user/usys.pl`
  - הוספנו `entry("memsize");`.

- בקובץ `user/user.h`
  - הוספנו `int memsize(void);`.

- הקובץ `user/memsize_test.c`
  - מדפיס זיכרון לפני הקצאה.
  - מקצה 20000 בייטים.
  - מדפיס זיכרון אחרי הקצאה.
  - משחרר את ההקצאה.
  - מדפיס זיכרון אחרי השחרור.

מדוע `myproc()->sz`:

- הקריאה `myproc()` מחזירה את ה-`struct proc` של ה-process הנוכחי.
- ל-`struct proc` יש שדה בשם `sz`.
- השדה `sz` הוא גודל זיכרון ה-process בבייטים.
- המטלה מבקשת את גודל הזיכרון של ה-process הרץ, כך שזהו מקור האמת הישיר.

פלט צפוי מהרצה שלנו:

```text
memory before allocation: 16384 bytes
memory after allocation: 81920 bytes
memory after free: 81920 bytes
```

למה הגודל לא עולה בדיוק ב-20000:

**מה זה `sbrk`:**

הקריאה `sbrk(n)` היא syscall שמרחיב את ה-heap של ה-process ב-`n` בייטים. ל-heap יש מצביע סיום שנקרא "program break". הקריאה `sbrk` מזיזה את המצביע הזה למעלה ב-`n` ומחזירה את המצביע הישן. ה-kernel אז מקצה את ה-pages הדרושים לכסות את טווח ה-heap החדש. הפונקציה `malloc` בנויה על גבי `sbrk`.

**שתי סיבות נפרדות לכך שהצמיחה גדולה מ-20000:**

*סיבה 1 — גרגיניות של pages:*

ה-kernel מקצה זיכרון פיזי page אחד בכל פעם (4096 בייטים לכל page).
אם הקריאה `sbrk(20000)` הייתה נקראת ישירות, ה-kernel היה מקצה 5 pages =
20480 בייטים. כלומר אפילו `sbrk` ישיר מגדיל את הזיכרון יותר מהמבוקש.

*סיבה 2 — גודל מינימלי של chunk ב-`morecore` (ההשפעה הגדולה יותר):*

הקריאה `malloc(20000)` לא קוראת ל-`sbrk(20000)`. כאשר ה-free list ריק,
הקריאה `malloc` מפעילה את `morecore`, שאוכפת מינימום של 4096 *יחידות header* לכל
קריאת `sbrk` (ראו שורות 52-53 ב-`umalloc.c`). זה מונע תשלום על עלות ה-syscall
בכל הקצאה קטנה.

כל `Header` על riscv64 הוא 16 בייטים:
- השדה `union header *ptr`: 8 בייטים (מצביע 64-ביט)
- השדה `uint size`: 4 בייטים
- 4 בייטים של padding (ליישור ה-struct ל-8 בייטים)

כך שקריאת `sbrk` המינימלית היא:

```
4096 units × 16 bytes/unit = 65536 bytes = 16 pages
```

הקצאת 20000 בייטים דורשת רק כ-1251 יחידות header, שהן מתחת למינימום של 4096,
לכן `morecore` מעלה ל-4096 וקוראת ל-`sbrk(65536)`. לכן `p->sz`
גדל מ-16384 ל-81920 (קפיצה של 65536 בייטים).

המינימום של 4096 הוא בחירת עיצוב של K&R להפחתת עלות syscall. הוא
חולק את אותו מספר עם גודל ה-page אך הם מודדים דברים שונים: אחד הוא
יחידות header, השני הוא בייטים.

**מדוע `malloc(20000)` שני לא מגדיל עוד את הזיכרון:**

אחרי ה-`malloc(20000)` הראשון, נשארים בערך 65536 − 20016 = 45520 בייטים
ב-free list. הקריאה `malloc(20000)` השנייה מוצאת מספיק מקום שם וגוזרת ממנו
ללא קריאה ל-`sbrk`. גודל ה-process נשאר זהה עד שה-free list יתרוקן שוב.

מדוע `free()` לא מקטינה את `memsize()`:

- הפונקציה `free()` מחזירה זיכרון ל-free list של ה-allocator ב-user space.
- היא לא קוראת ל-`sbrk` עם ערך שלילי.
- לכן ה-process עדיין מחזיק באותה כמות זיכרון מנקודת מבט ה-kernel.

מודל מחשבה שימושי: הקריאה `sbrk` היא כמו רוצ'ט חד-כיווני — היא רק עולה. הפונקציות `malloc`
ו-`free` מנהלות pool *בתוך* המרחב שכבר נתבע מה-kernel.
אחרי `free`, ה-allocator מסמן את הבלוק הזה כזמין שוב, כך שה-`malloc` הבא
יוכל לעשות בו שימוש חוזר ללא קריאה ל-`sbrk`. ה-kernel רואה רק את
קו המים הגבוה ביותר. הקריאה `memsize()` קוראת את קו המים הזה, כך שהוא נשאר ב-81920
בין אם ההקצאה שוחררה ובין אם לא.

מה לדעת לציון:

- הקריאה `memsize()` מחזירה גודל process כפי שה-kernel יודע, לא "בייטים של malloc שבשימוש כרגע".
- הפונקציות `malloc` ו-`free` הן פונקציות ספרייה של user-space ב-xv6.
- ה-kernel רואה רק את גודל ה-process שמנוהל דרך `sbrk` / `growproc`.

## משימה 3: `co_yield(pid, value)`

מטרת המשימה:

```c
int co_yield(int pid, int value);
```

ה-system call מאפשר לשני processes להחליף ערכים ולמסור ביצוע זה לזה.

ההתנהגות הנדרשת מה-PDF:

- הקורא נותן PID של מטרה וערך חיובי.
- אם המטרה טרם קראה ל-`co_yield`, הקורא נכנס לשינה.
- ברגע ששני ה-processes קראו ל-`co_yield`, הערכים מוחלפים.
- ה-CPU עובר ישירות מ-process אחד לשני.
- process שישן בתוך `co_yield` לא אמור להיבחר על ידי ה-scheduler הרגיל.
- בהצלחה, הפונקציה מחזירה את הערך שנשלח על ידי ה-process האחר.
- בשגיאה, הפונקציה מחזירה `-1`.

### אילוצים חשובים

המטלה אוסרת במפורש:

- הוספת שדות ל-`struct proc`.
- הוספת מצבי process.
- הוספת מבני נתונים גלובליים או ברמת ה-process.
- הסתמכות על מרובי מעבדים.

המטלה מחייבת:

- הגדרת QEMU למעבד אחד.
- שימוש ב-`swtch` ישיר למסירה הסופית.
- שמירה על מודל ה-processes הקיים של xv6 תקין.

הגדרנו:

```make
CPUS := 1
```

בקובץ `Makefile`.

### הבדיקה הנדרשת

הקובץ `user/co_test.c` עוקב אחר הבדיקה של ה-PDF:

```c
int pid1 = getpid();
int pid2 = fork();

if(pid2 == 0){
  for(;;){
    int value = co_yield(pid1, 1);
    printf("Child received: %d\n", value);
  }
} else {
  for(;;){
    int value = co_yield(pid2, 2);
    printf("parent received: %d\n", value);
  }
}
```

הפלט הצפוי חוזר על עצמו:

```text
parent received: 1
Child received: 2
parent received: 1
Child received: 2
...
```

התוכנית אינסופית בכוונה. הבדיקה מגדירה לולאות `for(;;)` אינסופיות — כך מוגדרת
בדיוק ב-PDF, וצוות הקורס אישר שהם בודקים רק את הגרסה עם הלולאה האינסופית.
הבוחן מריץ אותה כמה שניות, מוודא שהתבנית נכונה, ועוצר.

## עיצוב `co_yield`

מכיוון שאנחנו לא יכולים להוסיף שדות ל-`struct proc`, אנחנו עושים שימוש חוזר
בשדות קיימים לכל process:

- השדה `p->chan`
  - מסמן ש-process ישן בתוך `co_yield`.
  - שומר על מי הוא מחכה.

- השדה `p->trapframe->a1`
  - שומר את הערך שה-process הזה רוצה לשלוח.
  - זה עובד כי `a1` במקור מכיל את הארגומנט השני של ה-syscall.

- השדה `p->trapframe->a0`
  - שומר את ערך ההחזרה שאמור לחזור ל-user space.
  - הערך `-1` משמש כערך זמני של "טרם נענה".

ערוץ ההמתנה הפרטי הוא:

```c
(void*)&p->context
```

הכתובת הזו ייחודית לכל process ולא בשימוש על ידי קוד sleep/wakeup רגיל של xv6,
כך שהיא סמן טוב ל-"ממתין ל-process זה ב-co_yield".

## זרימת `co_yield`

ישנם שני מקרים עיקריים.

### מקרה 1: ה-process הראשון מגיע

דוגמה:

```text
parent calls co_yield(child_pid, 2)
child has not called co_yield yet
```

ה-process ההורה:

1. מוצא את ה-process הילד.
2. שומר את ערכו היוצא ב-`trapframe->a1`.
3. שומר `-1` ב-`trapframe->a0` כערך החזרה ממתין.
4. מגדיר את `chan` לערוץ הפרטי של הילד.
5. מגדיר את מצב עצמו ל-`SLEEPING`.
6. מגדיר את הילד ל-`RUNNING`.
7. מעדכן את `mycpu()->proc` לילד.
8. קורא:

```c
swtch(&parent->context, &child->context);
```

בנקודה זו ההורה אינו `RUNNABLE`, כך שה-scheduler הרגיל לא יבחר בו.
הילד גם לא נבחר על ידי ה-scheduler הרגיל; הקריאה ל-`co_yield` עוברת אליו ישירות.

זה חשוב כי הצוות הבהיר שהפתרון הסופי לא אמור להשתמש ב-`sched()`
בתוך `co_yield`. הקורא הראשון עדיין נכנס לשינה, אבל ה-CPU
ניתן ישירות ל-process המטרה כדי שיגיע ל-`co_yield` שלו.

### מקרה 2: ה-process השני מגיע

דוגמה:

```text
child calls co_yield(parent_pid, 1)
parent is already sleeping in co_yield waiting for the child
```

ה-process הילד:

1. מוצא את process ההורה.
2. בודק שההורה ישן על ערוץ הפרטי של הילד.
3. שומר את ערכו היוצא ב-`trapframe->a1`.
4. מעתיק את ערך ההורה היוצא לרגיסטר ההחזרה של הילד.
5. מעתיק את ערך הילד היוצא לרגיסטר ההחזרה של ההורה.
6. מגדיר את הילד ל-`SLEEPING`.
7. מגדיר את ההורה ל-`RUNNING`.
8. מעדכן את `mycpu()->proc` להורה.
9. קורא:

```c
swtch(&child->context, &parent->context);
```

זוהי המסירה הישירה. ה-scheduler לא בוחר את ה-process המטרה.
הקוד עובר ישירות מהקשר הילד להקשר ההורה.

מאוחר יותר, כאשר ההורה קורא שוב ל-`co_yield`, אותו מנגנון מעיר
את הילד.

## למה שינינו את ה-Scheduler

לוגיקת ה-scheduler המקורית של xv6 מניחה:

```text
scheduler picks process P
P eventually switches back to scheduler
scheduler releases P's lock
```

מעבר ישיר בין processes יכול לשבור הנחה זו.

דוגמה:

```text
scheduler starts process B
B direct-switches to A inside co_yield
A later returns to the scheduler
```

כעת ה-frame של ה-scheduler שהריץ במקור את B מחודש על ידי A. אם ה-scheduler
משחרר בעיוורון את ה-lock של B, הוא יכול לשחרר lock שגוי וליפול עם:

```text
panic: release
```

לכן שינינו את ה-scheduler לשחרר את ה-process הרשום כרגע על ה-CPU:

```c
ran = c->proc;
c->proc = 0;
release(&ran->lock);
```

הדבר שומר על התנהגות xv6 הרגילה תקינה, אך גם מטפל נכון
במקרה של מעבר ישיר.

זה לא שימוש ב-scheduler לבחירת מטרת ה-coroutine. מטרת ה-coroutine
עדיין נבחרת בתוך `co_yield`, והמסירה עדיין משתמשת ב-`swtch` ישיר.

## הבהרת הצוות לגבי שינויי ה-Scheduler

מחוץ לקובץ Moodle Q&A, הצוות הבהיר מאוחר יותר ש:

- פתרון עובד שנופל אחרי כמה איטרציות עם `panic: release`
  יתקבל.
- פתרון שמשנה את ה-scheduler לחזור ל-process הרלוונטי
  יתקבל גם הוא.
- פתרון נכון לחלוטין ללא בעיות אלה יתקבל גם הוא.

מדוע הם כנראה נתנו הבהרה זו:

- מעבר ישיר בין processes קשה להתאים להנחות ה-scheduler של xv6.
- סטודנטים רבים כנראה השיגו את החלפת הערכים ואת המעבר הישיר לעבוד, אבל
  נתקלו ב-`panic: release` אחרי timer interrupt או חזרה מאוחרת של ה-scheduler.
- המטלה המקורית לא הסבירה במלואה כיצד לשמור על הנחות בעלות ה-lock
  של ה-scheduler לאחר מעבר ישיר.

איך זה משפיע עלינו:

- זה רלוונטי כי שינינו את ה-scheduler בכוונה.
- השינוי שלנו הוא בדיוק בקטגוריה שהצוות אמר שמתקבלת.
- המימוש שלנו לא נפל בבדיקות.
- עלינו להסביר שהשינוי ב-scheduler הוא לנכונות ה-lock, לא לבחירת process המטרה.

הסבר טוב לציון:

```text
The handoff itself happens inside co_yield with swtch. We changed the scheduler
only because after a direct process switch, the process returning to the
scheduler may not be the same process originally selected by the scheduler.
So the scheduler releases c->proc, the process actually returning, instead of
the old loop variable.
```

הבהרה זו אינה אחת הרשומות הממוספרות ב-`moodle_qa.md`; היא
פורסמה בנפרד. אם שואלים עליה, אל תאמרו שהיא הגיעה מה-Moodle Q&A.

## טיפול בשגיאות

הפונקציה `co_yield` מחזירה `-1` עבור:

- PID שהוא אפס או שלילי.
- PID שהוא ה-PID של הקורא עצמו.
- PID שאינו קיים.
- process מטרה שנהרג.
- מטרה שהיא `UNUSED` או `ZOMBIE`.
- מטרה שישנה, אבל לא כ-peer הדדי `co_yield` צפוי.
- מטרה שנמצאת במצב שאינו שימושי לבדיקת coroutine מוגבלת זו של שני processes.

הקובץ `user/co_test.c` גם מריץ את שלושת מקרי השגיאה הנדרשים בהתחלה,
לפני כניסה ללולאת המסירה של ה-PDF. בתוך xv6 הרצת `co_test` פשוט מדפיסה:

```text
invalid pid: -1
self yield: -1
killed target: -1
parent received: 1
Child received: 2
...
```

שורות השגיאה מגיעות מ-`run_error_tests()` שנקראת ללא תנאי בראש
ה-`main()`. לולאת הורה/ילד האינסופית של ה-PDF רצה מיד לאחר מכן.

## הערות על Locking

כללים חשובים של xv6:

- השדות `state` ו-`chan` של process מוגנים על ידי `p->lock`.
- הקריאה `swtch()` שומרת את ה-kernel context הנוכחי ומשחזרת kernel context אחר.
- הקריאה `acquire()` מבטלת פסיקות דרך `push_off()`.

במסירה הישירה שלנו:

- ה-lock של process המטרה מוחזק דרך ה-`swtch`.
- ה-lock של ה-process הנוכחי משוחרר לפני המעבר.
- זה תואם למוסכמה של xv6 ש-process ממשיך עם ה-lock שלו מוחזק
  ואז משחרר אותו בנתיב ההמשך הרגיל.

מסירת ה-lock היא החלק העדין:

- הקורא הראשון `P` מכוון ל-process `C` ב-`RUNNABLE`.
- הקריאה `find_proc` מחזירה עם `C->lock` מוחזק.
- ה-process `P` מסמן את עצמו `SLEEPING`, משחרר את `P->lock`, ועובר ישירות ל-`C`.
- ה-process `C` ממשיך מחזיק ב-`C->lock`, שהיא המוסכמה הרגילה של xv6 אחרי context switch בסגנון scheduler.
- כאשר `C` מאוחר יותר קורא ל-`co_yield` בחזרה ל-`P`, הוא מוצא את `P` ישן, מגדיר
  ערכי החזרה, משחרר את `C->lock`, ועובר ישירות ל-`P`.
- ה-process `P` ממשיך מחזיק ב-`P->lock`, מנקה את `chan`, משחרר את `P->lock`, וחוזר
  מה-syscall.

מדוע זה חשוב:

- פסיקות timer לא אמורות להפריע לאמצע מעבר ישיר קריטי מבחינת lock.
- המטרה ממשיכה במצב שבו היא יכולה לסיים בבטחה את ה-syscall שלה
  ולשחרר את ה-lock שלה.

## סיור בקוד המימוש

סעיף זה עובר על כל חלק קוד שכתבנו למשימה 3, בסדר שבו הוא מתבצע בזמן ריצה.

---

### 1. הפונקציה `sys_co_yield` — נקודת הכניסה ל-syscall (`kernel/sysproc.c`)

```c
uint64
sys_co_yield(void)
{
  int pid;
  int value;

  argint(0, &pid);   // read first argument (pid) from trapframe->a0
  argint(1, &value); // read second argument (value) from trapframe->a1
  return co_yield(pid, value);
}
```

זהו wrapper דק בלבד. לכל syscall ב-xv6 יש כזה ב-`sysproc.c`. תפקידו היחיד
הוא לחלץ את הארגומנטים מרגיסטרי ה-trapframe (שם `ecall` של ה-user space
הכניס אותם) ולהעבירם למימוש האמיתי ב-`proc.c`.

---

### 2. הפונקציה `co_chan` — ערוץ השינה הפרטי (`kernel/proc.c`)

```c
static void*
co_chan(struct proc *p)
{
  return (void*)&p->context;
}
```

מנגנון ה-sleep/wakeup של xv6 מזהה *מדוע* process ישן לפי channel: ערך בגודל
מצביע השמור ב-`p->chan`. שני processes הישנים על אותו channel ניתנים להעיר ביחד.

אנחנו צריכים channel שמשמעותו "ישן בתוך co_yield ממתין ל-process P".
אנחנו משתמשים בכתובת של ה-struct של `context` של P. הכתובת הזו היא:

- ייחודית לכל process (היא נמצאת בתוך מערך `struct proc`).
- לעולם לא בשימוש על ידי קוד sleep/wakeup רגיל של xv6.

לכן אם רואים `p->chan == co_chan(target)`, זה אומר חד-משמעית ש-P
ישן בתוך `co_yield`, ממתין ל-`target` להגיע.

---

### 3. הפונקציה `find_proc` — איתור ונעילת המטרה (`kernel/proc.c`)

```c
static struct proc*
find_proc(int pid)
{
  struct proc *p;

  for(p = proc; p < &proc[NPROC]; p++){
    acquire(&p->lock);           // lock before reading any fields
    if(p->pid == pid && p->state != UNUSED)
      return p;                  // return with lock still held
    release(&p->lock);
  }

  return 0;                      // not found
}
```

אנחנו סורקים את טבלת ה-processes הגלובלית בחיפוש אחר process שה-PID שלו תואם
ושאינו `UNUSED` (חריץ שמעולם לא שימש או שפורר).

הפרט החשוב: אנחנו מחזירים עם ה-lock **עדיין מוחזק**. הקורא
אחראי לשחרר אותו. זהו הדפוס הסטנדרטי של xv6 ל-
"מצא ובדוק" — מחזיקים את ה-lock בזמן קריאה ושינוי שדות ה-process,
ומשחררים כשמסיימים.

---

### 4. הפונקציה `co_yield` — הלוגיקה המרכזית (`kernel/proc.c`)

```c
int
co_yield(int pid, int value)
{
  struct proc *p = myproc();   // the calling process
  struct proc *target;
  int ret;
```

הקריאה `myproc()` מחזירה את ה-process הרץ כרגע על ה-CPU הזה. המשתנה `p` הוא הקורא,
והמשתנה `target` יהיה ה-process שאנחנו רוצים למסור אליו.

#### אימות קלט

```c
  if(pid <= 0 || pid == p->pid)
    return -1;

  target = find_proc(pid);
  if(target == 0)
    return -1;

  if(target->killed || target->state == UNUSED || target->state == ZOMBIE){
    release(&target->lock);
    return -1;
  }
```

אנחנו דוחים:
- PID לא חוקי (אפס, שלילי, עצמי).
- PID שאינם קיימים בטבלת ה-processes.
- processes שנהרגו, אינם בשימוש, או שכבר zombie (יצאו אך טרם נאספו). אין
  טעם למסור ל-process מת.

אחרי ש-`find_proc` מחזיר, השדה `target->lock` מוחזק. אם חוזרים מוקדם עקב
שגיאה, חייבים לשחרר אותו קודם.

#### מקרה 1 — ה-peer כבר ישן (הקורא השני מגיע)

```c
  if(target->state == SLEEPING){
    if(target->chan != co_chan(p)){
      // Target is sleeping, but NOT waiting for us specifically.
      release(&target->lock);
      return -1;
    }

    acquire(&p->lock);
    if(p->killed){
      release(&p->lock);
      release(&target->lock);
      return -1;
    }
```

המטרה היא SLEEPING וה-channel שלה הוא `co_chan(p)`, כלומר היא נכנסה
לשינה בתוך `co_yield` ממתינה ספציפית לנו. אנחנו ה-process השני שמגיע לנקודת המפגש.

כעת אנחנו מחזיקים שני locks: השדה `target->lock` והשדה `p->lock`.

```c
    // Exchange values.
    p->trapframe->a1 = value;                      // save our outgoing value
    p->trapframe->a0 = target->trapframe->a1;      // we receive what target saved
    if(target->trapframe->a0 == (uint64)-1)
      target->trapframe->a0 = value;               // give target our value as its return
```

כל process שומר את ערכו היוצא ב-`trapframe->a1` (רגיסטר הארגומנט השני).
ערך ההחזרה נכנס ל-`trapframe->a0` (רגיסטר ערך ההחזרה). כאשר ה-process
ממשיך מ-`swtch` ובסופו של דבר חוזר מה-syscall, ה-kernel שם את `trapframe->a0`
ברגיסטר ההחזרה אוטומטית.

בדיקת ה-`-1`: כאשר הקורא הראשון נכנס לשינה (מקרה 2 למטה), הוא הגדיר את
ה-`a0` שלו ל-`-1` כ-placeholder שמשמעותו "עדיין לא נענה". הקורא השני
ממלא אותו כאן.

```c
    // Do the direct handoff.
    p->chan = co_chan(target);  // mark us as sleeping, waiting for target
    p->state = SLEEPING;
    target->state = RUNNING;
    target->chan = 0;           // clear target's wait marker — it's awake now
    mycpu()->proc = target;     // tell the CPU it's now running target

    release(&p->lock);
    swtch(&p->context, &target->context);  // jump to target's kernel stack
```

הקריאה `swtch` שומרת את כל הרגיסטרים ב-`p->context` ומשחזרת
אותם מ-`target->context`. הביצוע ממשיך בתוך `target` מ-
איפה שקרא לאחרונה ל-`swtch` — שהוא בתוך `co_yield`, ממש אחרי
קריאת ה-`swtch` שלו עצמו.

```c
    // We resume here when someone later co_yields back to us.
    ret = p->trapframe->a0;   // read the return value that was placed for us
    p->chan = 0;
    release(&p->lock);
    return ret;
  }
```

כאשר אנחנו בסופו של דבר מתעוררים (הפעם הבאה שה-peer קורא ל-`co_yield` בחזרה),
אנחנו ממשיכים ממש אחרי ה-`swtch`. בנקודה זו ערך ההחזרה שלנו כבר
נכתב ל-`p->trapframe->a0` על ידי מי שהעיר אותנו.

#### מקרה 2 — ה-peer הוא RUNNABLE (הקורא הראשון מגיע)

```c
  if(target->state != RUNNABLE){
    release(&target->lock);
    return -1;
  }

  acquire(&p->lock);
  if(p->killed){
    release(&p->lock);
    release(&target->lock);
    return -1;
  }
```

המטרה היא RUNNABLE — היא עדיין לא קראה ל-`co_yield`. אנחנו הראשונים
להגיע. אנחנו דוחים כל מצב אחר (למשל, מטרה שכבר ישנה על משהו לא קשור,
או RUNNING על מעבד אחר).

```c
  p->trapframe->a1 = value;   // save our outgoing value for when target arrives
  p->trapframe->a0 = -1;      // placeholder: "nobody has given me a return value yet"
  p->chan = co_chan(target);   // mark us as sleeping, waiting for target
  p->state = SLEEPING;

  target->state = RUNNING;    // skip RUNNABLE — direct switch, no scheduler
  mycpu()->proc = target;     // tell the CPU it's now running target

  release(&p->lock);
  swtch(&p->context, &target->context);  // jump directly to target
```

אנחנו חונים את עצמנו כ-SLEEPING ומריצים את המטרה ישירות. המטרה
היא כעת RUNNING מבלי שנבחרה אי פעם על ידי ה-scheduler. היא
בסופו של דבר תקרא ל-`co_yield` בחזרה, תמצא אותנו ישנים (מקרה 1), תבצע
את החלפת הערכים, ותעיר אותנו.

```c
  ret = p->trapframe->a0;   // value filled in by the peer when it woke us
  p->chan = 0;
  release(&p->lock);
  return ret;
}
```

אותו נתיב המשך כמו מקרה 1.

---

### 5. ה-`scheduler` המעודכן (`kernel/proc.c`)

השינוי היחיד ב-scheduler הקיים הוא זה:

```c
// Original xv6:
// release(&p->lock);

// Our version:
ran = c->proc;
c->proc = 0;
release(&ran->lock);
```

**מדוע זה חשוב:**

לולאת ה-scheduler הרגילה של xv6 בוחרת process בשם `p`, עוברת אליו, וכאשר
חוזרים משחררת את `p->lock`. הדבר עובד כאשר `p` הוא ה-process שחוזר
ל-scheduler.

עם מעבר ישיר, process שונה יכול לחזור ל-frame של ה-scheduler שהריץ במקור את `p`.
דוגמה:

```
scheduler picks process B
B calls co_yield, switches directly to A
A finishes and returns to the scheduler
```

כעת ה-frame של ה-scheduler שהריץ את `B` מחודש על ידי `A`. אם הוא משחרר
את `p->lock` (ה-lock של B), הוא יפול עם `panic: release` כי ה-CPU
לא מחזיק את ה-lock של B — הוא מחזיק את ה-lock של A.

התיקון: במקום לשחרר את `p->lock`, משחררים את `c->proc->lock` —
ה-lock של ה-process שנמצא בפועל על ה-CPU כרגע. זה תמיד ה-process
שחזר ל-frame הזה של ה-scheduler, ללא קשר לשאלה אם הוא היה ה-`p` המקורי
או מישהו אחר.

## מה בדקנו

הרצנו rebuild נקי:

```sh
make clean
make qemu
```

לאחר מכן בתוך xv6:

```text
helloworld
memsize_test
usertests -q
co_test
```

תוצאות שנצפו:

```text
Hello World xv6
```

```text
memory before allocation: 16384 bytes
memory after allocation: 81920 bytes
memory after free: 81920 bytes
```

```text
invalid pid: -1
self yield: -1
killed target: -1
```

הבדיקה `usertests -q` הסתיימה עם:

```text
ALL TESTS PASSED
```

הבדיקה `co_test` רצה כמה שניות והדפיסה שוב ושוב:

```text
parent received: 1
Child received: 2
```

ללא `panic: release`, ללא deadlock, וללא syscall לא ידוע.

הערה: הבדיקות ב-`usertests` מדפיסות בכוונה כמה הודעות `usertrap(): unexpected scause`
במהלך בדיקות הגנת זיכרון. הודעות אלה צפויות כל עוד הבדיקה מדפיסה `OK` ובסופו של דבר `ALL TESTS PASSED`.

## בדיקות מקומיות אופציונליות

אנחנו שומרים קובץ מקור נוסף לבדיקת עומסים לשימוש שלנו:

```text
local_tests/co_stress_test.c
```

הקובץ הזה בודק 2000 מסירות coroutine בשני מקרים:

- ההורה מגיע ל-`co_yield` ראשון.
- הילד מגיע ל-`co_yield` ראשון.

סטטוס: ה-plumbing של ה-Makefile שבנה בדיקה זו (דגל `EXTRA_TESTS=1`,
`EXTRA_TEST_BINS`, הכלל `_co_stress_test`) הוסר מה-Makefile הסופי
כדי לשמור על ה-diff של ההגשה מינימלי. המקור עדיין
נמצא ב-`local_tests/` כרפרנס. להריץ אותו שוב מקומית, יש לשחזר
את כלל הבנייה (כמה שורות: target בשם `_co_stress_test` שמקשר מ-
`local_tests/co_stress_test.o`, ורשומה ב-`UPROGS`).

להגשת Moodle הסופית, לא לכלול את `local_tests/`.

## שאלות שעלינו להיות מוכנים לענות

### במה system call שונה מקריאת פונקציה רגילה?

קריאת פונקציה רגילה נשארת ב-user space וקופצת לכתובת אחרת ב-user-space.

לעומת זאת, system call משתמש ב-`ecall`, נלכד ב-kernel, מריץ kernel handler, וחוזר
דרך מנגנון ה-trap. מספר ה-syscall נמצא ב-`a7`, ארגומנטים
נמצאים ברגיסטרים כמו `a0` ו-`a1`, וערך ההחזרה חוזר ב-`a0`.

### מה זה `usys.pl`?

הוא מייצר stubs של assembly עבור פונקציות syscall של user-space. לדוגמה,
הקריאה `entry("memsize")` מייצרת פונקציית `memsize` שמכניסה `SYS_memsize`
ל-`a7`, מריצה `ecall`, וחוזרת.

### למה `memsize()` מחזיר `myproc()->sz`?

כי `sz` הוא גודל זיכרון ה-process הנוכחי בבייטים, השמור ב-process control block.

### למה הזיכרון לא מתכווץ אחרי `free()`?

כי הפונקציה `free()` מחזירה זיכרון ל-allocator של user-space בלבד. היא לא
מחזירה בהכרח pages ל-kernel ולא מקטינה את `p->sz`.

### איך `co_yield` מעביר ערכים?

כל process שומר את ערכו היוצא ב-`trapframe->a1`. כאשר ה-process השני
מגיע, ה-kernel מעתיק כל ערך שמור לרגיסטר ההחזרה של ה-process השני,
`trapframe->a0`.

### איפה מצב ההמתנה של coroutine מאוחסן?

בשדות process קיימים:

- `state = SLEEPING`
- `chan = co_chan(peer)`

לא משמש שדה process חדש או טבלה גלובלית.

### איך `co_yield` עוקף את ה-scheduler?

ה-kernel מגדיר את process המטרה ל-`RUNNING`, מעדכן את `mycpu()->proc`, ו-
קורא ל-`swtch` ישירות מהקשר ה-process הנוכחי להקשר process המטרה. הדבר קורה גם
כאשר ה-peer כבר ישן בתוך `co_yield` וגם כאשר הקורא הראשון מריץ ישירות
peer ב-`RUNNABLE`.

### למה עדיין שינינו את ה-scheduler?

כי מעבר ישיר יכול לגרום ל-process שונה לחזור ל-frame ישן של ה-scheduler.
שחרור `c->proc->lock` משחרר את ה-lock של ה-process שחזר בפועל.

### האם זה מותר?

כן. הבהרת הצוות מקבלת במפורש פתרונות שמשנים את ה-scheduler
לחזור ל-process הרלוונטי.

### מה המגבלות של המימוש שלנו?

הוא מתוכנן לבדיקת המסירה ההדדית של שני processes של המטלה. הוא
לא מנסה לממש framework מלא של coroutine למטרות כלליות עבור processes רבים
וגרפי המתנה שרירותיים.

הדבר מתיישב עם ה-Q&A של הצוות, שאומר שהבדיקה שסופקה היא מה שהם
מתעניינים בו.

## רשימת תיוג להגשה סופית

תשובה קצרה:

יש להגיש את עץ המקור המלא של xv6 עם שינויי המטלה שלנו, אבל לא לכלול
קבצי לימוד/ניהול שלנו.

כלומר, מושגית זה:

```text
the whole xv6 codebase
minus files that are only for us, like the study README, Moodle Q&A, PDF, and agent notes
minus build artifacts
```

אין להגיש רק את הקבצים ששינינו. הבוחן אמור לקבל עץ xv6
שניתן לבנות בצורה רגילה עם `make qemu`.

### קבצים שחייבים להיות בהגשה

החבילה צריכה לכלול את עץ המקור הרגיל של xv6, כולל:

```text
Makefile
LICENSE
README
kernel/
mkfs/
user/
```

היא חייבת לכלול את שינויי המקור של המטלה שלנו:

```text
Makefile
kernel/defs.h
kernel/proc.c
kernel/syscall.c
kernel/syscall.h
kernel/sysproc.c
user/user.h
user/usys.pl
user/helloworld.c
user/memsize_test.c
user/co_test.c
```

הקובץ `user/co_test.c` חשוב כי המטלה מבקשת במפורש את תוכנית הבדיקה. בדיקות
מצבי השגיאה נמצאות באותה תוכנית: הן רצות ללא תנאי בתחילת `main()`,
לפני לולאת המסירה של ה-PDF.

### קבצים להוציא מההגשה

הקבצים האלה מיועדים ללימוד, הגדרה, או זרימת עבודה מקומית. הם לא צריכים להיות ב-
חבילת Moodle הסופית:

```text
.git/
.DS_Store
.claude/
ASSIGNMENT_README.md
moodle_qa.md
os262-assignment1.pdf
agent.md
local_tests/
```

גם אלה לא נדרשים לציון:

```text
.devcontainer/
.vscode/
```

יש גם להוציא artifacts של בנייה. הרצת `make clean` מסירה את
החשובים שבהם:

```text
fs.img
kernel/kernel
kernel/*.o
kernel/*.d
kernel/*.asm
kernel/*.sym
user/*.o
user/*.d
user/_*
user/*.asm
user/*.sym
mkfs/mkfs
```

### למה אין `co_error_test.c` נפרד

ה-PDF מבקש את קוד הבדיקה של משימה 3 ב-`user/co_test.c`, וגם מבקש
לבדוק תנאי שגיאה. כדי לשמור את ההגשה הסופית פשוטה, שניהם נמצאים ב-
`user/co_test.c`. הרצת `co_test` בתוך xv6:

1. תחילה מריצה `run_error_tests()`, שבודקת:

```text
non-existent PID
self-yield
killed PID
```

2. לאחר מכן נכנסת ללולאת הורה/ילד האינסופית של ה-PDF.

בוחן שמריץ `co_test` ללא ארגומנטים רואה את שניהם. אין מצב נפרד
או דגל argv.

### שלבי אריזה סופית מדויקים

לפני הגשה:

1. להחליט על מימוש איזה branch להשתמש אחרי השוואה עם השותף.
2. לממזג את הגרסה הסופית ל-branch ההגשה או ליצור עותק אריזה סופי.
3. להריץ:

```sh
make clean
make qemu
```

4. בתוך xv6, להריץ:

```text
helloworld
memsize_test
co_test
```

5. לצאת מ-QEMU.
6. להריץ:

```sh
make clean
```

7. ליצור ZIP/TAR מעץ המקור הנקי.
8. לוודא שה-ZIP/TAR לא כולל:

```text
.git/
ASSIGNMENT_README.md
moodle_qa.md
os262-assignment1.pdf
agent.md
.devcontainer/
.vscode/
local_tests/
build artifacts
```

9. להגיש את החבילה ל-Moodle.

### רצף פקודות אריזה מעשי

דרך נקייה אחת היא ליצור עותק זמני מחוץ ל-repo ולדחוס אותו.

מהספרייה ההורה של `xv6-riscv`, ליצור עותק נקי ששם התיקייה ברמה העליונה
שלו עדיין הוא `xv6-riscv`:

```sh
mkdir -p /tmp/os262-submit
rsync -a xv6-riscv/ /tmp/os262-submit/xv6-riscv/ \
  --exclude='.git' \
  --exclude='.DS_Store' \
  --exclude='.claude' \
  --exclude='ASSIGNMENT_README.md' \
  --exclude='moodle_qa.md' \
  --exclude='os262-assignment1.pdf' \
  --exclude='agent.md' \
  --exclude='local_tests' \
  --exclude='.devcontainer' \
  --exclude='.vscode'
```

לאחר מכן, בתוך התיקייה שהועתקה:

```sh
cd /tmp/os262-submit/xv6-riscv
make clean
```

לאחר מכן לוודא שהתיקייה שהועתקה עדיין בונה:

```sh
make qemu
```

בתוך xv6, להריץ:

```text
helloworld
memsize_test
co_test
```

לצאת מ-QEMU, לנקות שוב, ולדחוס:

```sh
make clean
cd /tmp/os262-submit
zip -r xv6-riscv.zip xv6-riscv
```

ה-ZIP צריך להכיל את `xv6-riscv/` עם קוד המקור בפנים.

### בדיקת תקינות סופית

לפני העלאה, לפרוק את החבילה במקום זמני ולבדוק:

```sh
make qemu
```

הנקודה החשובה היא שהתיקייה שהוגשה צריכה לבנות בעצמה. אם היא
בונה רק בגלל קבצים מחוץ ל-ZIP שהוגש, החבילה שגויה.
