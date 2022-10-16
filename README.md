# GREEN BROWSER

##  TASK 1

Durata implementarii: 2h

***Rezolvare:*** 
Pentru fiecare nume de fisier citit din fisierul `master.txt`, am deschis fisierul respectiv si am salvat informatiile in campurile structurii (functia `complete_database`).

##  TASK 2

Durata implementarii: 3h

***Rezolvare:*** 
Am despartit sirul introdus de la tastatura in cuvinte, pe care apoi le-am cautat in continutul fiecarui site, si am marcat rezultatele intr-un vector de frecventa. Am afisat URL-urile site-urilor selectate, in ordine.

##  TASK 3

Durata implementarii: 3.5h

***Rezolvare:*** 
Rezolvarea este asemanatoare celei precedente, de aceasta data tinand cont de secventele dintre ghilimele si de cuvintele precedate de caracterul *'-'*.

##  TASK 4

Durata implementarii: 3h

***Rezolvare:*** 
Pentru URL-urile unor site-uri citite de la tastatura am verificat daca se afla in fisierul `master.txt`, caz in care am calculat *checksum-ul* scriptului si l-am comparat cu cel salvat in structura. <br>
In fiecare dintre cazuri (site-ul nu se afla in master.txt, *checksum-ul* corespunde sau cele doua *checksum-uri* nu sunt egale), am afisat mesajul corespunzator.

##  TASK 5

Durata implementarii: 5h

***Rezolvare:*** 
Am creat o interfata grafica folosind biblioteca ***`ncurses`***, pentru un motor de cautare ce imbina task-urile precedente. <br>
Initial, se asteapta tastei *'C'*, dupa care se acceseaza bara de cautare in care se introduc cuvintele. Exista optiunea de *backspace* in caz ca se doreste stergerea unor litere intorduse. <br>
Dupa apasarea tastei *'Enter'*, exista 3 optiuni: `quit (Q)`, `cautare simpla (S)` si `cautare avansata (A)`. In cazul alegerii uneia dintre optiunile de cautare, este permisa utilizatorului glisarea printre site-urile selectate si alegerea unuia caruia i se va afisa titlul si continutul.
