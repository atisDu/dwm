/* See LICENSE file for copyright and license details. */

/* appearance */
//#include <cstddef>
static const unsigned int borderpx  = 2;        /* border pixel of windows (thinner = calmer) */
static const unsigned int snap      = 22;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 1;   	/* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */

static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "Fantasque Sans Mono:size=14" };
static const char dmenufont[]       = "Fantasque Sans Mono:size=14";
static const char col_gray1[]       = "#1d1f21";	     /*    "#1d1f21"; */
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#eeeeee";             /* "#bbbbbb"; */
static const char col_gray4[]       = "#eeeeee";	     /* "#eeeeee"; */
/*static const char col_cyan[]        = "#4c566a";*/
static const char col_cyan[]	    = "#040707";

static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

/* tagging */
static const char *tags[] = { "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX" };

static const Rule rules[] = {
	/* xprop(1):
   *	WM_CLASS(STRING) = instance, class
   *	WM_NAME(STRING) = title
	 */
	/* class       instance   title       tags mask   isfloating  iscentered  monitor  w, h */
	{ "Gimp",      NULL,      NULL,       0,          0,          0,          -1 },
	{ "Origin",    NULL,      NULL,       0,          1,          0,          -1 },
	{ "Onboard",   NULL,      NULL,       0,          1,          0,          -1 },
	{ "kcmshell6", NULL,      NULL,       0,          1,          1,          -1 },
	{ NULL,        "librepods", NULL,      0,          1,          1,          -1,     480, 360 },
    };

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "Režģis",      tile },    /* first entry is default */
	{ "Peld*",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "kitty", NULL };
static const char *mutecmd[] = { "/bin/sh", "-c", "pactl set-sink-mute @DEFAULT_SINK@ toggle; pkill -USR1 slstatus", NULL };
static const char *volupcmd[] = { "/bin/sh", "-c", "pactl set-sink-volume @DEFAULT_SINK@ +5%; pkill -USR1 slstatus", NULL };
static const char *voldowncmd[] = { "/bin/sh", "-c", "pactl set-sink-volume @DEFAULT_SINK@ -5%; pkill -USR1 slstatus", NULL };
static const char *mutemiccmd[] = { "/bin/sh", "-c", "pactl set-source-mute @DEFAULT_SOURCE@ toggle; pkill -USR1 slstatus", NULL };
static const char *brupcmd[] = { "/bin/sh", "-c", "brightnessctl set 5%+; pkill -USR1 slstatus", NULL };
static const char *brdowncmd[] = { "/bin/sh", "-c", "brightnessctl set 5%-; pkill -USR1 slstatus", NULL };


/* screenshots via KDE's Spectacle (already installed with Plasma) instead of flameshot */
static const char *screenshotfull[] = { "spectacle", "--background", "--nonotify", "--fullscreen", NULL };
static const char *screenshotsel[] = { "spectacle", "--background", "--nonotify", "--region", NULL };
static const char *screenshotfullclip[] = { "spectacle", "--background", "--nonotify", "--fullscreen", "--copy-image", NULL };
static const char *screenshotselclip[] = { "spectacle", "--background", "--nonotify", "--region", "--copy-image", NULL };
static const char *webcmd[] = { "firefox", NULL};
static const char *lockcmd[] = { "/home/atis/git/dwm/scripts/lock", NULL };
static const char *powermenucmd[] = { "/home/atis/git/dwm/scripts/powermenu", NULL };
static const char *powercmd[] = { "/bin/sh", "-c",
	"printf 'performance\\nbalanced\\npower-saver\\n' | dmenu -i -p 'power:' -fn 'Fantasque Sans Mono:size=14' -nb '#1d1f21' -nf '#eeeeee' -sb '#040707' -sf '#eeeeee' | xargs -r powerprofilesctl set; pkill -USR1 slstatus", NULL };
static const char *kbcmd[] = { "/bin/sh", "-c",
	"case $(printf 'latvian\\nfinnish\\n' | dmenu -i -p 'kbd:' -fn 'Fantasque Sans Mono:size=14' -nb '#1d1f21' -nf '#eeeeee' -sb '#040707' -sf '#eeeeee') in "
	"latvian) setxkbmap lv -variant apostrophe ;; "
	"finnish) setxkbmap -I/home/atis/.config/xkb -symbols 'fiqwerty(basic)' -print"
	" | xkbcomp -I/home/atis/.config/xkb -w 0 - \"$DISPLAY\" ;; "
	"esac", NULL };
/* zen mode: kitty gets a centered text column and a solid background */
static const int zenwidth = 1100;  /* px wide text column in zen mode; tune to taste */
static char zenpad[8] = "0";       /* horizontal padding in pts, computed by togglezen */
static const char *zenoncmd[] = { "/bin/sh", "-c",
	"printf 'background_opacity 1.0\\nwindow_padding_width 24 %s\\n' \"$1\" > ~/.config/kitty/zen.conf; pkill -USR1 kitty",
	"sh", zenpad, NULL };
static const char *zenoffcmd[] = { "/bin/sh", "-c",
	": > ~/.config/kitty/zen.conf; pkill -USR1 kitty", NULL };
static const char *obscmd[] = { "obsidian", NULL};
static const char *btcmd[] = { "/home/atis/git/dwm/scripts/btmenu", NULL };
static const char *wificmd[] = { "/home/atis/git/dwm/scripts/wifimenu", NULL };
static const char *displaycmd[] = { "/home/atis/git/dwm/scripts/screenmenu", NULL };
static const char *sttcmd[] = { "/home/atis/git/dwm/scripts/stt", NULL };

#include <X11/XF86keysym.h>

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY|ShiftMask,             XK_b,      spawn,          {.v = btcmd } },
	{ MODKEY|ShiftMask,             XK_r,      spawn,          {.v = wificmd } },
	{ MODKEY,                       XK_u,      spawn,          {.v = sttcmd } },
	{ MODKEY|ShiftMask,             XK_z,      togglezen,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_o,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_l,      spawn,          {.v = lockcmd} },
	{ MODKEY|ShiftMask,             XK_e,      spawn,          {.v = powercmd} },
	{ MODKEY|ShiftMask,             XK_Escape, spawn,          {.v = powermenucmd} },
	{ MODKEY|ShiftMask,             XK_w,      spawn,          {.v = kbcmd} },
	{ MODKEY|ShiftMask,             XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY|ShiftMask,             XK_f,      fullscreen,     {0} },
	/*{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },*/
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_c,      quit,           {0} },
	{ MODKEY,			XK_a,	   spawn,	   {.v = webcmd } },
    { MODKEY,			XK_w,	   spawn,	   {.v = obscmd } },
 	{ 0, 				XF86XK_AudioMute, 	   spawn,   	   {.v = mutecmd } },
	{ 0, 				XF86XK_AudioLowerVolume, 	   spawn, 	   {.v = voldowncmd } },
	{ 0, 				XF86XK_AudioRaiseVolume, 	   spawn, 	   {.v = volupcmd } },
	{ 0, 				XF86XK_MonBrightnessUp, 	   spawn, 	   {.v = brupcmd} },
	{ 0, 				XF86XK_MonBrightnessDown, 	   spawn, 	   {.v = brdowncmd} },
	{ 0,                XF86XK_AudioMicMute, 	   spawn,   	   {.v = mutemiccmd } },
	{ MODKEY|ShiftMask, XF86XK_Display, 		   spawn,   	   {.v = displaycmd } }, 
	{ 0,                XF86XK_Display, 		   spawn,   	   {.v = displaycmd } },
	{ 0,				XK_Print,  spawn,	   {.v = screenshotfull} },
	{ MODKEY,			XK_p,	   spawn,	   {.v = screenshotsel} },
	{ ShiftMask,			XK_Print,  spawn,	   {.v = screenshotfullclip} },
	{ MODKEY|ShiftMask,		XK_p,	   spawn,	   {.v = screenshotselclip} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

