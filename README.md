
# Tema 4 - HTTP

Pentru aceasta tema am folosit scheletul de la laboratorul 9.

Pentru comenzile de citire, citesc cu fgets si pun terminatorul de sir
pentru ultimul caracter. Apoi verific fiecare comanda folosind strcmp.
Am declarat un buffer[1024], vectorul de Cookies, numarul lor si un
token utilizat pentru librarii.

Pentru parsarea Json am folosit biblioteca parson sugerata in enuntul temei.
Cred ca e destul de self explanatory de ce am folosit-o, lucrand in C. Plus,
a fost si usor de inteles din README-ul de pe site.

## Comenzile

1. Comanda register
   * daca suntem logati, atentionam utilizatorul sa se delogheze intai
   * se deschide conexiunea si se citeste de la tastatura folosind fgets
   username-ul si parola utilizatorului
   * se verifica ca acestea sa nu contina spatii, sa se fi primit ceva sau
   ca primul caracter sa nu fie spatiu, caz in care comanda se va opri
   si utilizatorul va trebui sa o tasteze iar daca vrea sa se inregistreze
   * se parseaza json-ul, se trimite cererea de tip post si verificam daca
   raspunsul primit este cu succes sau daca username-ul deja exista si printam
   un mesaj corespunzator situatiei

2. Comanda login
   * daca suntem logati, atentionam utilizatorul sa se delogheze intai
   * se deschide conexiunea si se citeste de la tastatura folosind fgets
   username-ul si parola utilizatorului (+ aceeasi verificare ca la register)
   * se parseaza jsonul, se trimite cererea de tip post si introducem 
   cookie-urile in vectorul de cookies, care se gasesc la linia "Set-Cookie: "
   folosind strstr. Vrem sa luam exact cookie-ul, asa ca incepem de la adresa
   p + 12 (lungimea pana la token), pana cand intalnim caracterul ';'.
   * la final se printeaza un mesaj corespunzator situatiei respective

3. enter_library
   * se deschide conexiunea si se trimite cererea de tip get
   * daca nu avem parte de o eroare(utilizatorul nu este logat), se extrage
   tokenul pentru a-l putea folosi pentru task-urile urmatoare
   * in fiecare caz se printeaza un mesaj corespunzator

4. get_books
   * se deschide conexiunea si se trimite cererea de tip get la care se va
   adauga si token-ul primit anterior pentru a functiona
   * daca cererea a fost cu succes, verificam sa nu fie goala lista, caz
   in care vom parsa Json-ul pentru un aspect mai placut (functiile sunt
   extrase din biblioteca parson.c), altfel vom preciza ca nu este nicio
   carte in biblioteca
   * in functie de cele 3 tipuri, se va printa mesajul corespunzator

5. get_book
   * se citeste de la tastatura id-ul folosind fgets si se verifica
   ca acesta sa fie numar, folosind functia isNumber, in care am verificat
   ca fiecare caracter sa fie o cifra folosind isdigit (din <ctype.h>)
   * creez url-ul folosindu-ma de id-ul primit si trimit cererea de tip get
   in functie de caz printez mesajul corespunzator, si daca totul este ok
   se parseaza cartea ca la comanda anterioara

6. add_book
   * se citesc de la tastatura titlul, autorul, genul cartii, editura si
   numarul de pagini, pentru cel din urma verificandu-se ca acesta sa
   fie un numar si pentru restul se verifica sa se fi primit ceva de la
   tastatura sau ca primul caracter sa nu fie spatiu
   * se parseaza json-ul, se trimite cererea de tip post, iar la final voi
   afisa un mesaj corespunzator

7. delete_book
   * se citeste de la tastatura id-ul folosind fgets si se verifica
   ca acesta sa fie numar
   * creez url-ul folosindu-ma de id-ul primit si trimit cererea de tip
   delete, iar in functie de caz printez mesajul corespunzator

8. logout
   * se deschide conexiunea si se trimite cererea de tip get, golesc
   variabilele in care se afla token-ul si cookie-urile si afisez
   un mesaj corespunzator in functie de caz

9. exit
   * se termina programul (break)

10. Daca se citeste de la tastatura altceva, utilizatorul va fi anuntat
ca sunt doar 9 tipuri de comenzi disponibile. Variabilele sunt dezalocate
in final.

### Alte detalii despre implementare

* serialize_user(char **string, char *username, char *password)
* serialize_book(char **string, char *title, char *author,
char *genre, char *publisher, int page_count)

Pentru aceste 2 functii am vizualizat README-ul bibliotecii
parson.c in care se explica cum se creeaza json-ul si conform
task-urilor, este nevoie de un json care contine username si
password si unul care contine detaliile despre carte.

* Functiile de get, post au fost incepute la laborator, la care am mai adaugat
headerul de authorization pentru mesaj si un parametru pentru token. Delete
este la fel ca GET, dar in loc de GET se foloseste DELETE.

* wrong_string verifica sa nu fie string-ul gol sau sa inceapa cu ' '

* isNumber verifica daca parametrul primit este numar pozitiv
