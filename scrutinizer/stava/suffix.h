/* Rättstavningsprogram. Version 2.55 2001-10-29
   Copyright (C) 1990-2001
   Joachim Hollman och Viggo Kann
   joachim@nada.kth.se viggo@nada.kth.se
*/


/* InitSuf initierar suffixtabellen och returnerar 0 om det gick bra. */
extern int InitSuf(const char *SLfilename);
/* CheckSuffix kollar om word innehåller suffix i suffixtabellen. För varje
rad i suffixtabellen som stämmer överens kollas att ordet finns i ordlistan
om suffixet byts ut mot alla kollsuffix i så fall returneras 1. Annars
returneras 0. */
extern int CheckSuffix(const unsigned char *word, int tryallrules);

