/* Rättstavningsprogram. Version 2.55 2001-10-29
   Copyright (C) 1990-2001
   Joachim Hollman och Viggo Kann
   joachim@nada.kth.se viggo@nada.kth.se
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


/* InitSuf initierar suffixtabellen och returnerar 0 om det gick bra. */
extern int InitSuf(const char *SLfilename);
/* CheckSuffix kollar om word innehåller suffix i suffixtabellen. För varje
rad i suffixtabellen som stämmer överens kollas att ordet finns i ordlistan
om suffixet byts ut mot alla kollsuffix i så fall returneras 1. Annars
returneras 0. */
extern int CheckSuffix(const unsigned char *word, int tryallrules, int compoundSearch);

