## Sugubete Andrei 315CA Automatica si Calculatoare
## Tema 1 - Structuri de Date

Am folosit comentarii in engleza pentru ca codul sa fie usor inteles si pentru persoane care nu vorbesc romana.

# Detalii implementare comenzi:

- ALLOC_ARENA:
Funcția alloc_arena C returnează un pointer către o structură arena_t nou alocată.

Mai întâi alocă memorie pentru structura arena_t folosind calloc. Dacă alocarea eșuează, funcția returnează un pointer NULL.

Apoi, alocă memorie pentru câmpul alloc_list al structurii arena_t folosind calloc, își setează câmpurile dimensiunea și data_size. Dacă alocarea eșuează, funcția eliberează memoria alocată anterior pentru structura arena_t și returnează un pointer NULL.

În cele din urmă, funcția returnează adresa noii structuri arena_t alocate.

- DEALLOC_ARENA:
Această funcție C dezaloca memoria alocată unei structuri de arenă eliberând toate blocurile, miniblocurile și listele asociate cu aceasta. Mai întâi eliberează datele miniblocurilor iterând peste lista de miniblocuri din fiecare bloc și eliberând rw_buffer, apoi datele miniblocului și, în final, nodul miniblocului. Apoi eliberează minilistul, blochează datele și blochează nodul. Acest proces se repetă pentru toate blocurile din arenă și, în final, arena și lista de alocări sunt eliberate.

- ALLOC_BLOCK:
Această funcție alloc_block() alocă un bloc de memorie într-o arenă. Mai întâi verifică dacă poate aloca blocul în funcție de adresa și dimensiunea solicitate. Apoi verifică dacă există vreun bloc deja alocat la adresa respectivă și returnează un mesaj de eroare dacă este cazul.

De asemenea, caută orice blocuri adiacente care pot fi îmbinate cu noul bloc pentru a minimiza fragmentarea memoriei. Odată ce îmbinarea este completă, noul bloc este alocat cu miniblocurile corespunzătoare, iar lista de alocări este actualizată cu noul bloc. Dacă este primul bloc care trebuie alocat, acesta indică spre sine.

În general, funcția alocă memorie în timp ce încearcă să minimizeze fragmentarea și se asigură că adresa de memorie corectă este alocată.

- FREE_BLOCK:
Această funcție free_block() este folosită pentru a elibera un bloc de memorie alocat anterior dintr-o arenă dată. Este nevoie de două argumente: arena_t *arena, care este un pointer către arena care conține blocul de memorie care urmează să fie eliberat și adresa const uint64_t, care este adresa de pornire a blocului de eliberat.

Funcția caută mai întâi blocul care conține adresa de memorie specificată, iterând peste lista de blocuri alocate în arenă. Dacă blocul nu este găsit, acesta afișează un mesaj de eroare și revine. Dacă blocul este găsit, funcția caută mini-blocul din blocul care conține adresa de memorie specificată. Odată ce găsește mini-blocul, elimină conținutul mini-blocului și îl eliberează. Funcția ajustează apoi dimensiunea blocului și lista de mini-blocuri în consecință.

Dacă lista de mini-blocuri devine goală după eliberarea mini-blocului, funcția eliberează și blocul și nodul de bloc corespunzător din lista de blocuri alocată arenei. Dacă mini-blocul care se eliberează este capul listei de mini-blocuri, funcția setează adresa de pornire a blocului la adresa de pornire a noului mini-bloc principal. Dacă mini-blocul care este eliberat este coada listei de mini-blocuri, funcția revine pur și simplu. În caz contrar, funcția creează o nouă listă de blocuri și mini-blocuri și transferă toate mini-blocurile din lista originală în lista nouă. În cele din urmă, funcția adaugă noul bloc la lista de blocuri alocată în poziția corectă și ajustează dimensiunea listei de blocuri alocate. Dacă adresa de memorie specificată nu se află în niciunul dintre blocurile alocate, funcția imprimă un mesaj de eroare și revine.

- READ:
Această funcție preia o structură arena_t, o adresă și o dimensiune ca parametri și citește date din locația adresei pentru dimensiunea specificată. Mai întâi caută blocul care conține adresa, apoi găsește primul minibloc din acel bloc care se suprapune cu intervalul de adrese solicitat. Apoi, funcția citește datele din minibloc într-un buffer și le trimite la consolă. Dacă dimensiunea cerută este mai mare decât dimensiunea blocului, funcția va emite un mesaj de avertizare și va citi numai câte date sunt disponibile în bloc. Dacă adresa solicitată nu este găsită în niciun bloc, funcția emite un mesaj de eroare.

- WRITE:
Această funcție preia un pointer arena_t, o adresă întregă nesemnată de 64 de biți, o dimensiune a unui întreg nesemnat de 64 de biți și un pointer către o matrice de date cu numere întregi de 8 biți. Funcția scrie datele la adresa specificată în arenă.

Funcția caută mai întâi blocul care conține adresa dată folosind o listă dublu legată de blocuri din arenă. Dacă blocul nu este găsit, emite un mesaj de eroare. Dacă blocul este găsit, funcția continuă să găsească miniblocul care conține adresa dată și să scrie datele în acesta. Dacă dimensiunea datelor este mai mare decât dimensiunea blocului, funcția scrie doar spațiul disponibil și emite un mesaj de avertizare.

Funcția nu returnează nicio valoare, dar scrie un mesaj de avertizare dacă este necesar sau un mesaj de eroare dacă adresa specificată este invalidă.

- PMAP:
Această funcție C ia ca argument un pointer către o structură arena_t și emite diverse statistici legate de utilizarea memoriei arenei.

Funcția imprimă mai întâi dimensiunea totală a arenei și apoi iterează peste fiecare bloc din arenă pentru a calcula cantitatea de memorie liberă, numărul de blocuri alocate și numărul de miniblocuri alocate.

După aceea, funcția iterează peste fiecare bloc și imprimă informații despre fiecare, inclusiv adresele de început și de sfârșit ale blocului și informații despre fiecare dintre miniblocurile sale, cum ar fi adresele de început și de sfârșit și dacă au permisiuni de citire/scriere.

# Functii auxiliare:

- SIM_ERRORER:
Funcția sim_errorer ia ca intrare un număr întreg și afișează mesajul „Comandă nevalidă. Vă rugăm să încercați din nou”. de un număr de ori folosind o buclă. În esență, este un simplu generator de mesaje de eroare care repetă mesajul de eroare de câte ori este cerut de parametrul num.

- ERROER:
Această funcție se numește „errorer” și ia trei argumente: un întreg „num”, un șir „tok” și un șir constant „delim”. Funcția afișează un mesaj de eroare „Comandă nevalidă. Vă rugăm să încercați din nou”. pentru fiecare token din șirul „tok” folosind funcția „printf”. Funcția apelează apoi funcția „strtok” pentru a tokeniza șirul „tok” folosind delimitatorul specificat în argumentul „delim”, până când nu mai rămân token-uri. În cele din urmă, funcția apelează o altă funcție numită "sim_errorer" cu întregul "num" ca argument.

- FIX_MY_BLOCK:
Funcția ia un pointer către o structură block_t, care are o listă dublu legată de miniblocuri. Funcția accesează primul și ultimul minibloc din listă pentru a determina adresa de pornire și dimensiunea blocului. Apoi ajustează dimensiunea blocului calculând diferența dintre adresa de început a primului minibloc și adresa de final a ultimului minibloc. În cele din urmă, actualizează adresa de început și dimensiunea blocului în consecință. În rezumat, funcția fixează începutul și dimensiunea unui bloc pe baza adreselor de început și de sfârșit ale primului și ultimului minibloc din lista sa de miniblocuri.
