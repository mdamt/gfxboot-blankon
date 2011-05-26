% - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
%
% Language selection dialog.
%
% - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


% - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
% Some global vars.
%

% fallback if there is no "languages"
/lang.items [ "en_US" ] def
/lang.names [ "English (US)" ] def

/.la.locale 0 def
/.la.keymap 1 def
/.la.rtl    2 def
/.la.name   3 def

/lang.defaultnames [
  [ "ace_ID" "us"    false   "Bahsa Achéh" ]
  [ "ban_ID" "us"    false   "Basa Bali" ]
  [ "btk_ID" "us"    false   "Hata Batak" ]
  [ "bug_ID" "us"    false   "Basa Ugi" ]
  [ "bew_ID" "us"    false   "Basé Betawi" ]
  [ "jv_ID" "us"    false   "Basa Jawa" ]
  [ "min_ID" "us"    false   "Baso Minangkabau" ]
  [ "ms_MY"  "us"    false   "Bahasa Melayu" ]
  [ "en_US"  "us"    false   "English (US)" ]
  [ "id_ID"  "us"    false   "Bahasa Indonesia" ]
  [ "zh_CN"  "us"    false   "简体中文" ]
] def


% - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
% Get language property.
%
% Returns .undef if not found.
%
% ( locale prop ) ==> ( obj )
%
/lang.getprop {
  lang.defaultnames {
    dup .la.locale get 3 index eq { exch get exch pop return } { pop } ifelse
  } forall

  % try a loose match
  lang.defaultnames {
    dup .la.locale get 3 index locale.cmp { exch get exch pop return } { pop } ifelse
  } forall

  pop pop .undef
} def


% - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
% Get language name.
%
% Returns locale if not found.
%
% ( locale ) ==> ( name )
%
/lang.getdefname {
  dup
  .la.name lang.getprop
  dup .undef ne { exch } if
  pop
} def


% - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
% Check for rtl.
%
% ( locale ) ==> ( bool )
%
/lang.getrtl {
  .la.rtl lang.getprop
  dup .undef eq { pop false } if
} def


% - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
% Parse "languages" file.
%
% ( ) ==> ( )
%
/lang.parsedata {
  /lang.default 0 def

  "languages" findfile dup { /lang.data exch def } { pop return } ifelse

  /la.tmp.datalen lang.data length def
  /la.tmp.str lang.data cvs def
 
  la.tmp.datalen 0 eq { return } if
  la.tmp.str la.tmp.datalen 1 sub get '\n' ne { return } if

  '\n' seteotchar

  /lang.items [

    /la.tmp.len 0 def
    /la.tmp.cnt 0 def
    {
      la.tmp.str la.tmp.len add strdup
      dup dup length 0 put
      /la.tmp.len over length 1 add la.tmp.len add def

      dup 0 get '*' eq { 1 add /lang.default la.tmp.cnt def } if

      la.tmp.len la.tmp.datalen ge { exit } if

      /la.tmp.cnt inc
    } loop

  ] def

  ' ' seteotchar

  /lang.names [

    lang.items {

      dup dup length add

      dup 0 get {
        dup 0 0 put 1 add
        exch pop
      } {
        pop lang.getdefname
      } ifelse

    } forall

    keymap.submenu {
      /lang.submenu.keyboard
    } if

  ] def

  /lang.submenu.idx keymap.submenu { lang.names length 1 sub } { -1 } ifelse def

  0 seteotchar

} def


/lang.submenu.keyboard.buf 64 string def
/lang.submenu.keyboard {
  /txt_keyboard exec "\x09\x09%s" lang.submenu.keyboard.buf sprintf
  lang.submenu.keyboard.buf
} def


% - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
% Build language list.
%
% ( ) ==> ( )
%
/lang.init {
  /xmenu.lang .xm_size array def
  /xmenu xmenu.lang def

  lang.parsedata

  xmenu .xm_current lang.default put
  xmenu .xm_list lang.names put

  xmenu .xm_title /txt_language put

  % make menu smaller if there are more than 19 language entries
  %lang.items length 19 gt {
  %  xmenu .xm_vspace 2 put
  %} if

  % start with current lang

  /la.tmp.cnt 0 def
  lang.items {
    config.lang locale.cmp { xmenu .xm_current la.tmp.cnt put exit } if
    /la.tmp.cnt inc
  } forall

  /la.tmp.cnt 0 def
  lang.items {
    config.lang eq { xmenu .xm_current la.tmp.cnt put exit } if
    /la.tmp.cnt inc
  } forall

  % needed in case menu is only attached to lang menu
  keymap.init
} def


% - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
% Update language.
%
% ( ) ==> ( )
%
/lang.update {
  /xmenu xmenu.lang def

  xmenu .xm_current get lang.submenu.idx eq {
    xmenu .xm_current over .xm_last get put
    panel.keymap
  } {
    lang.items xmenu .xm_current get get
    dup
    setlang { /window.action actRedraw def } if
    getkeymapfromlocale setkeymap
    keymap.setdefault
  } ifelse

} def


% - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
% Show language menu.
%
% ( ) => ( )
%
/panel.lang {
  "keytable" help.setcontext

  window.xmenu
  dup .xmenu xmenu.lang put
  dup .xmenu.update /lang.update put
  dup window.init
      window.show
} def


% - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
% Return width of panel entry.
%
% ( ) => ( width )
%
/panel.lang.width {
  /xmenu xmenu.lang def

  pmenu.width
} def


% - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
% Redraw panel entry.
%
% ( panel ) => ( )
%
/panel.lang.update {
  /xmenu xmenu.lang def

  pmenu.panel.update

  xmenu.keymap .xm_panel_x xmenu.lang .xm_panel_x get put
} def


