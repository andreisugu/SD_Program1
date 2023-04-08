**Nume: Sugubete Andrei**
**Grupă: 315CA**

## Virtual Memory Allocator - Tema 1

# Mentiuni generale:
Am folosit comentarii in engleza pentru ca codul sa fie usor inteles si pentru persoane care nu vorbesc romana.

# Detalii implementare comenzi:

- ALLOC_ARENA:
Funcția "alloc_arena" în limbajul C aloca o structură "arena_t" și returnează un pointer către aceasta. Mai întâi se alocă memoria pentru structura "arena_t" și apoi pentru câmpul "alloc_list" al acestei structuri. Dacă alocarea de memorie eșuează, funcția returnează un pointer NULL, în caz contrar ea returnează adresa noii structuri "arena_t" alocate.

- DEALLOC_ARENA:
Funcția C dezalocă memoria unei structuri de tip arenă prin eliberarea tuturor blocurilor, miniblocurilor și listelor asociate. Procesul începe prin eliberarea datelor miniblocurilor, urmată de eliberarea minilistelor și a nodurilor asociate. Această acțiune se repetă pentru fiecare bloc din arenă, iar în final sunt eliberate arena și lista de alocări.

- ALLOC_BLOCK:
Funcția alloc_block() alocă un bloc de memorie într-o arenă, verificând dacă poate fi alocat blocul solicitat și dacă există vreun bloc deja alocat la adresa respectivă. De asemenea, încearcă să minimizeze fragmentarea memoriei prin îmbinarea cu blocurile adiacente. Funcția se asigură că adresa corectă de memorie este alocată și actualizează lista de alocări.

- FREE_BLOCK:
Funcția free_block() eliberează un bloc de memorie alocat anterior dintr-o arenă dată. Are nevoie de două argumente: arena_t *arena și adresa const uint64_t. Funcția caută blocul care conține adresa de memorie specificată, elimină mini-blocul din bloc și ajustează dimensiunea blocului și lista de mini-blocuri în consecință. Dacă lista de mini-blocuri devine goală după eliberare, funcția eliberează și blocul și nodul de bloc corespunzător din lista de blocuri alocată arenei. Dacă adresa de memorie specificată nu se află în niciunul dintre blocurile alocate, funcția afișează un mesaj de eroare și revine.

- READ:
Funcția primește o structură de tip arena_t, o adresă și o dimensiune. Ea caută blocul care conține adresa și găsește primul minibloc din acel bloc care se suprapune cu intervalul de adrese solicitat. Datele din minibloc sunt citite într-un buffer și trimise la consolă. Dacă dimensiunea cerută este mai mare decât dimensiunea blocului, funcția va emite un mesaj de avertizare și va citi numai câte date sunt disponibile în bloc. Dacă adresa solicitată nu este găsită în niciun bloc, funcția emite un mesaj de eroare.

- WRITE:
Funcția primește un pointer către o zonă de memorie (arena_t), o dimensiune și un pointer către o matrice de date. Ea caută blocul din arena care conține adresa specificată și încearcă să scrie datele în miniblocul respectiv. Dacă dimensiunea datelor este mai mare decât dimensiunea blocului, funcția scrie doar spațiul disponibil și emite un mesaj de avertizare. Funcția nu returnează nicio valoare, dar emite mesaje de avertizare sau de eroare în cazul în care adresa specificată este invalidă sau blocul nu este găsit.

- PMAP:
Funcția C primește un pointer către o structură arena_t și afișează diverse statistici legate de utilizarea memoriei arenei, incluzând dimensiunea totală a arenei și cantitatea de memorie liberă, numărul de blocuri și numărul de miniblocuri alocate. De asemenea, afișează informații despre fiecare bloc din arenă, inclusiv adresele de început și de sfârșit ale blocului și informații despre fiecare dintre miniblocurile sale, cum ar fi adresele de început și de sfârșit și permisiunile de citire/scriere ale miniblocurilor.

# Functii auxiliare:

- SIM_ERRORER:
Funcția sim_errorer ia ca intrare un număr întreg și afișează mesajul „Comandă nevalidă. Vă rugăm să încercați din nou”. de un număr de ori folosind o buclă. În esență, este un simplu generator de mesaje de eroare care repetă mesajul de eroare de câte ori este cerut de parametrul num.

- ERROER:
Funcția "errorer" primește trei argumente: un întreg "num", un șir "tok" și un șir constant "delim". Funcția afișează un mesaj de eroare pentru fiecare token din șirul "tok" folosind funcția "printf". Apoi, funcția tokenizează șirul "tok" folosind delimitatorul specificat în argumentul "delim" prin apelarea funcției "strtok". În cele din urmă, funcția apelează funcția "sim_errorer" cu întregul "num" ca argument.

- FIX_MY_BLOCK:
Funcția primește un pointer către o structură block_t cu o listă dublu legată de miniblocuri. Ea accesează primul și ultimul minibloc pentru a calcula adresa de început și dimensiunea blocului. Acestea sunt ajustate și actualizate în consecință. În general, funcția fixează începutul și dimensiunea unui bloc pe baza primului și ultimului minibloc din lista sa de miniblocuri.
