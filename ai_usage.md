Pentru aceasta etapa a proiectului, am folosit Gemini Pro(Google) 
2. Prompturi pentru AI

Am trimis cerinta proiectului si i-am specificat sa rezolve doar partea de filter.I-am dat si structura de date din cod 

typedef struct { int id; char inspectorName[50]; 

GPS gps; 

char category[30]; 

int severity; 

time_t timestamp; 

char description[100]; 

} REPORT;

Prompt:
cerinta+ te rog să imi generezi functia parse_condition care realizeaza separarea 'field:operator:value' în 3 bucati si functie  match_condition care returnează 1 dacă raportul respecta conditia si 0 altfel.

3. Ce a generat

Pentru parse_condition, a folosit aritmetica pointerilor si functia strchr pentru a gasi separatorul : si a extras campurile folosind strncpy.
Pentru match_condition, a scris o serie de conditii if/else if care comparau parametrul field cu numele campurilor din structura, convertind apoi parametrul value în tipul de date corespunzator și aplicand operatorul logic extras.
4. Schimbari

AI-ul a interpretat gresit numele variabilei pentru inspector  (ex: r->inspector). A trebuit sa modific manual acest camp in r->inspectorName pentru a se potrivi exact cu definitia structurii mele .

5. Ce am invatat

Am invatat de ce AI-ul a facut cast la (time_t)atoll(value) pentru parsarea timpului. Deoarece time_t este pe 64 de biti, utilizarea unui simplu atoi() ar fi trunchiat imestamp..
