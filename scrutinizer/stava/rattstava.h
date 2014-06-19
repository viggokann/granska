/* Rättstavningsprogram. Version 2.63 2013-04-03
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

/* GenerateSimpleCorrections genererar rangordnade rättelseförslag på avstånd 1 i EL och IL från 
   ett potentiellt riktigt stavat ord word. Rättelseförslagen läggs i cset.
   Returnerar 0 om inget förslag kunde genereras och 1 annars. */
extern int GenerateSimpleCorrections(struct correctionSet *cset, unsigned char *word);
/* GenerateCorrections genererar rangordnade rättelseförslag till word.
   Rättelseförslagen läggs i cset.
   Returnerar 0 om inget förslag kunde genereras och 1 annars. */
extern int GenerateCorrections(struct correctionSet *cset, unsigned char *word);
