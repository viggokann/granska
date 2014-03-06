/* Rättstavningsprogram. Version 2.60 2004-09-05
   Copyright (C) 1990-2004
   Joachim Hollman och Viggo Kann
   joachim@algoritmica.se viggo@nada.kth.se
*/

/* wordSeparator är den sträng som skrivs ut mellan två rättelseförslag. */
const extern unsigned char *wordSeparator;

/* rattstava.h - gränssnitt till rattstava.c */
/* InitRattstava öppnar fyrgramsfilen och initierar hjälpstrukturer.
   separator är den sträng som skrivs ut mellan två rättelseförslag. */
extern int InitRattstava(const char *fyrgramfilename, 
			 const unsigned char *separator);
/* LagraFyrgram ser till att ett ords alla fyrgram är tillåtna */
INLINE extern void LagraFyrgram(const unsigned char *ord);
/* FyrKollaHela kollar om ett ords alla fyrgram är tillåtna */
INLINE extern int FyrKollaHela(const unsigned char *ord);

/* SimpleCorrections genererar rättelser på avstånd 1 i EL och IL från 
   ett potentiellt riktigt stavat ord. Returnerar 0 om inget förslag kunde
   genereras och 1 annars. */
extern int SimpleCorrections(unsigned char *word);
/* SkrivForslag genererar rättstavningsförslag för ett ord och skriver 
   ut dom med SkrivOrd(). Returnerar 0 om inget förslag kunde
   genereras och 1 annars. */
extern int SkrivForslag(unsigned char *ordin);
