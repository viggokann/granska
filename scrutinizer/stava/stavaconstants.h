/* Rättstavningsprogram. Version 2.55  2001-09-26
   Copyright (C) 1990-2001
   Joachim Hollman och Viggo Kann
   joachim@nada.kth.se viggo@nada.kth.se
*/

unsigned char ISO_intern[256] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
' ',0,0,0,0,0,0,0,0,0,0,0,0,'-',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
'P','Q','R','S','T','U','V','W','X','Y','Z',0,0,0,0,0,
0,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
'p','q','r','s','t','u','v','w','x','y','z',0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
'A','A','A','A','[',']','[','C','E',94,'E','E','I','I','I','I',
'D','N','O','O','O','O','\\',0,'\\','U','U','U','Y','Y',0,0,
'a','a','a','a','{','}','{','c','e',126,'e','e','i','i','i','i',
'd','n','o','o','o','o','|',0,'|','u','u','u','y','y',0,'y'};

unsigned char transform_diacritics[256] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
' ',0,0,0,0,0,0,0,0,0,0,0,0,'-',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
'P','Q','R','S','T','U','V','W','X','Y','Z',0,0,0,0,0,
0,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
'p','q','r','s','t','u','v','w','x','y','z',0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
'A','A','A','A',(unsigned char) 'Ä',(unsigned char) 'Å',(unsigned char) 'Ä','C','E',(unsigned char) 'É','E','E','I','I','I','I',
'D','N','O','O','O','O',(unsigned char) 'Ö',0,(unsigned char) 'Ö','U','U','U','Y','Y',0,0,
'a','a','a','a',(unsigned char) 'ä',(unsigned char) 'å',(unsigned char) 'ä','c','e',(unsigned char) 'é','e','e','i','i','i','i',
'd','n','o','o','o','o',(unsigned char) 'ö',0,(unsigned char) 'ö','u','u','u','y','y',0,'y'};

unsigned char MAC_intern[256] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,'-',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
'P','Q','R','S','T','U','V','W','X','Y','Z',0,0,0,0,0,
0,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
'p','q','r','s','t','u','v','w','x','y','z',0,0,0,0,0,
'[',']',0,0,0,'\\',0,0,0,0,'{',0,'}',0,0,0,0,0,0,0,0,0,0,0,0,0,'|',0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

unsigned char DOS_intern[256] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,'-',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
'P','Q','R','S','T','U','V','W','X','Y','Z',0,0,0,0,0,
0,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
'p','q','r','s','t','u','v','w','x','y','z',0,0,0,0,0,
0,0,0,0,'{',0,'}',0,0,0,0,0,0,0,'[',']',0,0,0,0,'|',0,0,0,0,'\\',0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

unsigned char intern_ISO[256] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
' ',0,0,0,0,0,0,0,0,0,0,0,0,'-',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
'P','Q','R','S','T','U','V','W','X','Y','Z',
0xc4 /* Ä (AE) */, 0xd6 /* Ö (OE) */, 0xc5 /* Å (AA) */, 0xc9 /* É */,0,
0,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
'p','q','r','s','t','u','v','w','x','y','z',
0xe4 /* ä (ae) */, 0xf6 /* ö (oe) */, 0xe5 /* å (aa) */, 0xe9 /* é */,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

unsigned char MAC_ISO[256] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,'-',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
'P','Q','R','S','T','U','V','W','X','Y','Z',0,0,0,0,0,
0,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
'p','q','r','s','t','u','v','w','x','y','z',0,0,0,0,0,
(unsigned char)'Ä',(unsigned char)'Å',0,0,0,(unsigned char)'Ö',0,0,
0,0,(unsigned char)'ä',0,(unsigned char)'å',0,0,0,
0,0,0,0,0,0,0,0,0,0,(unsigned char)'ö',0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

unsigned char DOS_ISO[256] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,'-',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
'P','Q','R','S','T','U','V','W','X','Y','Z',0,0,0,0,0,
0,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
'p','q','r','s','t','u','v','w','x','y','z',0,0,0,0,0,
0,0,0,0,(unsigned char)'ä',0,(unsigned char)'å',0,
0,0,0,0,0,0,(unsigned char)'Ä',(unsigned char)'Å',
0,0,0,0,(unsigned char)'ö',0,0,0,0,(unsigned char)'Ö',0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

unsigned char ISO_ISO[256] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

unsigned char ASCII_intern[256] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
'-',0,0,0,0,0,0,0,0,0,0,0,0,'-',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
'P','Q','R','S','T','U','V','W','X','Y','Z','[','\\',']',0,0,
0,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
'p','q','r','s','t','u','v','w','x','y','z','{','|','}',0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

unsigned char dubbelBokstavsTabell[256] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
'-',0,0,0,0,0,0,0,0,0,0,0,0,'-',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
'P','Q','R','S','T','U','V','W','X','Y','Z','[','\\',']',0,0,
0,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
'p','q','r','s','t','u','v','w','x','y','z','{','|','}',0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
'A','A','A','A','[',']','[','C','E',94,'E','E','I','I','I','I',
'D','N','O','O','O','O','\\',0,'\\','U','U','U','Y','Y',0,0,
'a','a','a','a','{','}','{','c','e',126,'e','e','i','i','i','i',
'd','n','o','o','o','o','|',0,'|','u','u','u','y','y',0,'y'};

unsigned char intern_gram[256] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
DELIMGRAM,DELIMGRAM,DELIMGRAM,DELIMGRAM,DELIMGRAM,DELIMGRAM,DELIMGRAM,DELIMGRAM,
DELIMGRAM,DELIMGRAM,DELIMGRAM,DELIMGRAM,DELIMGRAM,DELIMGRAM,DELIMGRAM,DELIMGRAM,
DELIMGRAM,DELIMGRAM,DELIMGRAM,DELIMGRAM,DELIMGRAM,DELIMGRAM,DELIMGRAM,DELIMGRAM,
DELIMGRAM,DELIMGRAM,DELIMGRAM,DELIMGRAM,DELIMGRAM,DELIMGRAM,DELIMGRAM,DELIMGRAM,
DELIMGRAM,'A'-'A','B'-'A','C'-'A','D'-'A','E'-'A','F'-'A','G'-'A',
'H'-'A','I'-'A','J'-'A','K'-'A','L'-'A','M'-'A','N'-'A','O'-'A',
'P'-'A','Q'-'A','R'-'A','S'-'A','T'-'A','U'-'A','V'-'A','W'-'A',
'X'-'A','Y'-'A','Z'-'A',91-'A',92-'A',93-'A',94-'A',DELIMGRAM,
DELIMGRAM,'a'-'a','b'-'a','c'-'a','d'-'a','e'-'a','f'-'a','g'-'a',
'h'-'a','i'-'a','j'-'a','k'-'a','l'-'a','m'-'a','n'-'a','o'-'a',
'p'-'a','q'-'a','r'-'a','s'-'a','t'-'a','u'-'a','v'-'a','w'-'a',
'x'-'a','y'-'a','z'-'a',123-'a',124-'a',125-'a',126-'a',DELIMGRAM,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
'A'-'A','A'-'A','A'-'A','A'-'A','['-'A',']'-'A','['-'A','C'-'A',
'E'-'A',94-'A','E'-'A','E'-'A','I'-'A','I'-'A','I'-'A','I'-'A',
'D'-'A','N'-'A','O'-'A','O'-'A','O'-'A','O'-'A','\\'-'A',DELIMGRAM,
'\\'-'A','U'-'A','U'-'A','U'-'A','Y'-'A','Y'-'A',DELIMGRAM,DELIMGRAM,
'a'-'a','a'-'a','a'-'a','a'-'a','{'-'a','}'-'a','{'-'a','c'-'a',
'e'-'a',126-'a','e'-'a','e'-'a','i'-'a','i'-'a','i'-'a','i'-'a',
'd'-'a','n'-'a','o'-'a','o'-'a','o'-'a','o'-'a','|'-'a',DELIMGRAM,
'|'-'a','u'-'a','u'-'a','u'-'a','y'-'a','y'-'a',DELIMGRAM,'y'-'a'};

unsigned char intern_p[256] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
DELIMP /* space */, DELIMP /* ! */, DELIMP /* " */, 0,
0, 0, DELIMP /* & */, DELIMP /* ' */,
DELIMP /* ( */, DELIMP /* ) */, 0, DELIMP /* + */,
DELIMP /* , */, DELIMP /* - */, DELIMP /* . */, DELIMP /* / */,
DELIMP /* 0 */, DELIMP, DELIMP, DELIMP, DELIMP, DELIMP, DELIMP, DELIMP,
DELIMP /* 8 */, DELIMP /* 9 */, DELIMP /* : */, 0, 0, 0, 0, DELIMP /* ? */,
0,'A','B','C','D','E','F','G',
'H','I','J','K','L','M','N','O',
'P','Q','R','S','T','U','V','W',
'X','Y','Z',91,92,93,94,0,
0,'a','b','c','d','e','f','g',
'h','i','j','k','l','m','n','o',
'p','q','r','s','t','u','v','w',
'x','y','z',123,124,125,126,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
'A','A','A','A','[',']','[','C',
'E',94,'E','E','I','I','I','I',
'D','N','O','O','O','O','\\',0,
'\\','U','U','U','Y','Y',0,0,
'a','a','a','a','{','}','{','c',
'e',126,'e','e','i','i','i','i',
'd','n','o','o','o','o','|',0,
'|','u','u','u','y','y',0,'y'};

unsigned char bindebokstav[128] = {
/* 's' står för bokstäver som i sammansättningar kan få s efter sig */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,' ','s','s','s',' ','s','s','s',' ','s','s','s','s','s','s',
's','s','s',' ','s',' ','s','s',' ',' ',' ',' ',' ',' ',' ',0,
0,' ','s','s','s',' ','s','s','s',' ','s','s','s','s','s',' ',
's','s','s',' ','s',' ','s','s',' ',' ',' ',' ',' ',' ',' ',0};

/* isConsonant talar om vilka tecken som är konsonanter */
unsigned char isConsonant[256] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,'B','C','D',0,'F','G','H',0,'J','K','L','M','N',0,
'P','Q','R','S','T',0,'V','W','X',0,'Z',0,0,0,0,0,
0,0,'b','c','d',0,'f','g','h',0,'j','k','l','m','n',0,
'p','q','r','s','t',0,'v','w','x',0,'z',0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,'C',0,0,0,0,0,0,0,0,
'D','N',0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,'c',0,0,0,0,0,0,0,0,
'd','n',0,0,0,0,0,0,0,0,0,0,0,0,0,0};

struct EntityTable {
  char *entity;
  unsigned char letter;
};
#define NOOFENTITIES 62
/* latin1Entities översätter HTML-&-namn till bokstäver */
static struct EntityTable latin1Entities[NOOFENTITIES] = {
{"AElig", (unsigned char)'Æ'}, /* capital AE diphthong (ligature) */
{"Aacute", (unsigned char)'Á'}, /* capital A, acute accent */
{"Acirc", (unsigned char)'Â'}, /* capital A, circumflex accent */
{"Agrave", (unsigned char)'À'}, /* capital A, grave accent */
{"Aring", (unsigned char)'Å'}, /* capital A, ring */
{"Atilde", (unsigned char)'Ã'}, /* capital A, tilde */
{"Auml", (unsigned char)'Ä'}, /* capital A, dieresis or umlaut mark */
{"Ccedil", (unsigned char)'Ç'}, /* capital C, cedilla */
{"ETH", (unsigned char)'Ð'}, /* capital Eth, Icelandic */
{"Eacute", (unsigned char)'É'}, /* capital E, acute accent */
{"Ecirc", (unsigned char)'Ê'}, /* capital E, circumflex accent */
{"Egrave", (unsigned char)'È'}, /* capital E, grave accent */
{"Euml", (unsigned char)'Ë'}, /* capital E, dieresis or umlaut mark */
{"Iacute", (unsigned char)'Í'}, /* capital I, acute accent */
{"Icirc", (unsigned char)'Î'}, /* capital I, circumflex accent */
{"Igrave", (unsigned char)'Ì'}, /* capital I, grave accent */
{"Iuml", (unsigned char)'Ï'}, /* capital I, dieresis or umlaut mark */
{"Ntilde", (unsigned char)'Ñ'}, /* capital N, tilde */
{"Oacute", (unsigned char)'Ó'}, /* capital O, acute accent */
{"Ocirc", (unsigned char)'Ô'}, /* capital O, circumflex accent */
{"Ograve", (unsigned char)'Ò'}, /* capital O, grave accent */
{"Oslash", (unsigned char)'Ø'}, /* capital O, slash */
{"Otilde", (unsigned char)'Õ'}, /* capital O, tilde */
{"Ouml", (unsigned char)'Ö'}, /* capital O, dieresis or umlaut mark */
{"THORN", (unsigned char)'Þ'}, /* capital THORN, Icelandic */
{"Uacute", (unsigned char)'Ú'}, /* capital U, acute accent */
{"Ucirc", (unsigned char)'Û'}, /* capital U, circumflex accent */
{"Ugrave", (unsigned char)'Ù'}, /* capital U, grave accent */
{"Uuml", (unsigned char)'Ü'}, /* capital U, dieresis or umlaut mark */
{"Yacute", (unsigned char)'Ý'}, /* capital Y, acute accent */
{"aacute", (unsigned char)'á'}, /* small a, acute accent */
{"acirc", (unsigned char)'â'}, /* small a, circumflex accent */
{"aelig", (unsigned char)'æ'}, /* small ae diphthong (ligature) */
{"agrave", (unsigned char)'à'}, /* small a, grave accent */
{"aring", (unsigned char)'å'}, /* small a, ring */
{"atilde", (unsigned char)'ã'}, /* small a, tilde */
{"auml", (unsigned char)'ä'}, /* small a, dieresis or umlaut mark */
{"ccedil", (unsigned char)'ç'}, /* small c, cedilla */
{"eacute", (unsigned char)'é'}, /*small e, acute accent */
{"ecirc", (unsigned char)'ê'}, /* small e, circumflex accent */
{"egrave", (unsigned char)'è'}, /* small e, grave accent */
{"eth", (unsigned char)'ð'}, /* small eth, Icelandic */
{"euml", (unsigned char)'ë'}, /* small e, dieresis or umlaut mark */
{"iacute", (unsigned char)'í'}, /* small i, acute accent */
{"icirc", (unsigned char)'î'}, /* small i, circumflex accent */
{"igrave", (unsigned char)'ì'}, /* small i, grave accent */
{"iuml", (unsigned char)'ï'}, /* small i, dieresis or umlaut mark */
{"ntilde", (unsigned char)'ñ'}, /* small n, tilde */
{"oacute", (unsigned char)'ó'}, /* small o, acute accent */
{"ocirc", (unsigned char)'ô'}, /* small o, circumflex accent */
{"ograve", (unsigned char)'ò'}, /* small o, grave accent */
{"oslash", (unsigned char)'ø'}, /* small o, slash */
{"otilde", (unsigned char)'õ'}, /* small o, tilde */
{"ouml", (unsigned char)'ö'}, /* small o, dieresis or umlaut mark */
{"szlig", (unsigned char)'ß'}, /* small sharp s, German (sz ligature) */
{"thorn", (unsigned char)'þ'}, /* small thorn, Icelandic */
{"uacute", (unsigned char)'ú'}, /* small u, acute accent */
{"ucirc", (unsigned char)'û'}, /* small u, circumflex accent */
{"ugrave", (unsigned char)'ù'}, /* small u, grave accent */
{"uuml", (unsigned char)'ü'}, /* small u, dieresis or umlaut mark */
{"yacute", (unsigned char)'ý'}, /* small y, acute accent */
{"yuml", (unsigned char)'ÿ'} /* small y, dieresis or umlaut mark */
};
