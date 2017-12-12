# Saptamana 1

Am elaborat cerintele proiectului, principalele obiective ale acestuia fiind:
* pe baza unui timetable sa dea de mancare la pesti (ore predeterminate)
* sa schimbe saptamanal apa la pesti
* sa porneasca luminile ziua si sa le opreasca seara
* (optional) notificare BT legata se statusul general

# Saptamana 2

Am determinat ca pentru golirea acvariului vom folosi un motor electric si 
o electrovalva, iar cantitatea de apa pentru golire va fi predeterminata pe
baza de timpul de functionare a motorului. De asemenea vom folosi un senzor
de nivel (maxim) ca o a doua metoda de siguranta. Un al doilea senzor va fi
folosit un butorul in care se va varsa apa uzata, ca o a doua metoda de 
siguranta.

# Saptamana 3

Am implementat algoritmul de utilizare a RTC-ului ca pe baza caruia se vor
porni task-urile legate de apa, hranit si orice alta implementare. Pentru a 
nu porni WDT-ul blocarea task-urilor se va face cu functionalitatile RTOS-ului
, pentru ca o interogare de tip IF(falg) s-a demonstrat prin testare ca in
anumite circumstante poate sa nu functione cum trebuie.

Ora de la RTC se va prelua la compilarea programului, deci va fi in urma cu 
cateva secunde fata de ore exacta, dar acest aspect in cadrul proiectului
nu este un impedimente pentru ca actiunile trebuie executate la un Dt intre ele
nu neaparat la ore prestabilite orar. - In aceasta privinta se poate 
implementa ulerior o functie care sa faca update manual (via serial) la ora.

# Saptamana 4

Am determinat ca senzosul de apa ales nu este suficient de exact pentru a 
determina nivelul apei, deci il vom folosi doar ca sistem de alerta de atingere
de nivel. Din cauza faptului ca este capacitiv si reactioneaza si cu
umiditatea din aer, nu poate face detectia la orice >0% deci detectia va
fi setata ca orice valoare >=20% (in mod evident cu un debounce realizat
pe un interval de minim 500ms, suficient pentru a fi mai rapid decat 
umplerea acvarilui).

# Saptamana 5 - 6

@TODO schema mecanica a mecanismului de hranire si printarea lui 3D.
@TODO implementarea in cod (RTOS) a functionalitatii de hranire si
senzoristica.

# Saptamana 7

Am achizitionat 4 electrovalve, le-am verificat, am facut cablajul aferent montajului.
Am achizitionat 3 relee de controlate la 5V(output - 10A @220V) pentru comanda electrovalvelor
si a pompei de apa ambele actionate @220V.

# Saptamana 8

Am achizitionat furtun pentru realizarea circuitului apei:
* scoaterea apei uzate din acvariu intr-un recipient extern
* umplerea cu apa filtrata si tratata a acvariului
Realizarea fizica a circuitului de apa si testarea lui.

# Saptamana 9 - 11

Realizarea fizica finala a intregului proiect si testarea lui, ca elemente discrete,
urmand integrarea lor in cadrul sistemului de operare facut in pasii anteriori.