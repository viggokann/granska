/* R�ttstavningsprogram. Version 2.63 2013-04-03
   Copyright (C) 1990-2013
   Joachim Hollman och Viggo Kann
   joachim@algoritmica.se viggo@nada.kth.se
*/

/******************************************************************************

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

******************************************************************************/

struct correctionSet {
  int noOfCorrections;
  unsigned char **corrections;
};

/* wordSeparator �r den str�ng som skrivs ut mellan tv� r�ttelsef�rslag. */
const extern unsigned char *wordSeparator;

/* rattstava.h - gr�nssnitt till rattstava.c */
/* InitRattstava �ppnar fyrgramsfilen och initierar hj�lpstrukturer.
   separator �r den str�ng som skrivs ut mellan tv� r�ttelsef�rslag. */
extern int InitRattstava(const char *fyrgramfilename, 
			 const unsigned char *separator);
/* LagraFyrgram ser till att ett ords alla fyrgram �r till�tna */
void LagraFyrgram(const unsigned char *ord);
/* FyrKollaHela kollar om ett ords alla fyrgram �r till�tna */
int FyrKollaHela(const unsigned char *ord);

/* GenerateSimpleCorrections genererar rangordnade r�ttelsef�rslag p� avst�nd 1 i EL och IL fr�n 
   ett potentiellt riktigt stavat ord word. R�ttelsef�rslagen l�ggs i cset.
   Returnerar 0 om inget f�rslag kunde genereras och 1 annars. */
extern int GenerateSimpleCorrections(struct correctionSet *cset, unsigned char *word);
/* GenerateCorrections genererar rangordnade r�ttelsef�rslag till word.
   R�ttelsef�rslagen l�ggs i cset.
   Returnerar 0 om inget f�rslag kunde genereras och 1 annars. */
extern int GenerateCorrections(struct correctionSet *cset, unsigned char *word);

extern int KollaLjudbyten(unsigned char *ord);


