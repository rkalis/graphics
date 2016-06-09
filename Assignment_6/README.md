STUDENT 1:
 * Student name .... Robert Jan Schlimbach
 * Student email ... robertjan.schlimbach@student.uva.nl
 * Student ID ...... 10802126

STUDENT 2:
 * Student name .... Rosco Kalis
 * Student email ... rosco.kalis@student.uva.nl
 * Student ID ...... 10771603

Unity version: 5.3.3f1,910d71450a97

Extra features:
 * Achtergrondmuziek
 * Geluidseffecten bij schieten, springen en het vernietigen van vijanden
 * Door one-way platforms naar beneden springen
 * Game Over screen met tekst gebaseerd op outcome en een play again knop

 * We vonden het stom om maar een projectile per keer te kunnen schieten
   dus dat hebben we niet geïmplementeerd. Dit zou je kunnen doen door
   te kijken of je een GameObject met tag/name kan vinden met GameObject.Find()
   Alleen als deze nog niet bestaat kan er geschoten worden.
   Dit ter illustratie dat we de implementatie kunnen bedenken, maar het dus
   een bewuste keuze is om het niet te implementeren want het is veel leuker
   om bullets te spammen.

Intsructies:
 * WASD of Arrow keys om te bewegen
 * Spatie om te schieten
 * Je hebt drie levens, je verliest een leven als je een vijand raakt.
 * Aan het einde van het level staat een gele finish-plaat, zodra je deze
   raakt heb je gewonnen
 * Er zijn normale platforms en one way platforms, bij one way platforms
   kun je er van onder doorheen springen, en weer naar beneden met S of 
   Down-arrow. Bij normale platforms kan dit niet. Er is geen visueel
   verschil tussen de twee.