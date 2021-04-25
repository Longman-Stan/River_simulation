ReadMe

Tema1 SPG
Vlad-Constantin Lungu-Stan 341C4

	O tema interesanta per total, putin frustranta pe alocuri daca vrei sa-ti iasa ceva bine, 
dar si in momentul in care iti iese te poti declara pe deplin mulumit. Sa incepem.

1. SkySphere(Bonus)
	Implementarea temei am pornit-o cu un bonus, anume implementarea unui Sky Sphere in loc de 
cube Map. Pentru ca nu mi-a placut rezolutia pe care o avea sfera oferita in frameworkul de
laborator mi-am facut una, parametrizata, care suporta orice rezolutie vreau(evident, in limita
de vertexi suportata de framework, un numar care incape pe un unsigned short). Dupa ce am
implementat-o si am gasit o textura suficient de buna amm trecut la implementarea restului temei.

2. Raul
	Prima si cea mai mare problema a fost generarea raului. Pentru ca am facut normal mapping pe
suprafata sa, alaturi de valuri(alte 2 bonusuri), informatia peer vertex a crescut simtitor, asa 
ca geometry shaderul a ajuns repede la limitari harware incat nu puteam sa generez decat vreo 40
de vertexi. Asta e o problema majora si oricum nu puteam nici inainte de a incarca vertexii cu 
informatie sa generez tot raul. Asa ca am parametrizat fiecare curba si am ajuns s-o randez pe 
bucati cat sa incapa. Asta mi-a oferit posibilitatea sa fac fiecare brat al raului din mai multe
curbe bezier, nu una singura, oferind un rau mai serpuit si mai credibil(alt bonus).
	Raul curge catre z pozitiv, translatia fiind facuta pe Ox. Pentru a nu ma chinui cu intersectii
in mod inutil si pentru a avea realism am impartit capatul raului in 3 bucati de marime egala, 
modificabil, evident, in functia de initializare. Punctul de intersectie este, astfel, un punct cu-
noscut apriori de pe marginea bratului principal. Acest lucru simplifica generarea muntilor si nu
schimba cu mare lucru implementarea(nu te mai chinui cu intersectii inutile care nu aduc absolut
nimic).]

3. Reflexia apei
	Reflexia apei a fost un subiect mai dificil pentru ca am implementat un spheremap, asa ca maparea
environmentului nu a mai fost asa usoara precum la laborator, insa am facut transformarile necesare
pentru a obtine maparea corecta intr-un final.

4. Muntii
	Sunt generati stiind marginile raurilor, 2 margini pentru muntii dintre brate si marginile late-
rale pentru toti ceilalti. Pentru muntii laterali fac suprafete de translatie de la marginea raului.
Pentru muntii interiori unesc punctele cu aceiasi parametrii de pe cate o margine a unui brat si 
adaug si media lor aritmetica pentru a sugera ideea de munte(sa am un punct care sa poata fi inaltat).
Deesigur, am aplicat texturi. Tema fiind despre fiorduri norvegiene, am ales o textura cu zapada.

5. Miscarea barcii
	Miscarea barcii se face aleatoriu, pornind de la capatul bratului principal catre punctul de centru.
Cand ajung in punctul de la capat, barca decide aleatoriu daca se intoarce sau daca porneste pe unul 
dintre celelalte brate. Cand ajunge la un capat se reintoarce catre punctul de centru, procesul de alegere
fiind reluat. Pentru dinamism(alt bonus), prin apasarea tastei F(irst person) te poti plasa in barca si sa
observi imprejurimile. Camera se va misca precum barca, fix ca si cum ai fi intr-o barca reala. 
	De asemenea generez cu un sistem de particule stropii pe care barca ii face in urma. Punctul de origine
este mereu fundul barcii, la intersectia cu apa. Stropii sunt generati in sens opus directiei din momentul
respectiv, simuland realist modul in care barca arunca apa in jur(alt bonus). In momentul in care o particula
atinge suprafata apei, acesta este regenerata cu aceeasi viteza, dar cu punctul de pornire fiind, dupa cum am
spus, fundul barcii si cu directia vitezei opusa directiei barcii din acel moment.

6. Profunzimea
	Pentru depth of field randez scena din perspectiva barcii intr-un frame buffer diferit, obtinand in acelasi
timp textura de depth. Dupa aceea creez o noua camera, ortografica si randez pe ecran un quad texturat cu
imaginea data, modificata in functie de depth. Trecerea dintre clar si blurat se face treptat, realist.

7. Zapada(bonus)
	Pentru ca este iarna, iubesc zapada si implementam fiorduri norvegiene am decis sa folosesc un alt sistem de
particule pentru a simula ninsoarea. Astfel fulgi de nea tot cad din cer. In momentul in care ating pamantul,
fulgul de nea renaste. Pentru a avea o miscare realista a fulgilor(desi functiona suficient de bine doar o cadere
liniara), am decis sa le induc o miscare sinusoidala. Efectul este unul foarte frumos, din punctul meu de vedere.


