## Czym jest problem komiwojażera?

Problem komiwojażera w skrócie TSP (travelling salesman problem) koncepcyjnie jest bardzo prosty. Sprzedawca, który wyrusza z miasta rodzinnego (miasto początkowe) chce sprzedać swój towar w określonej ilości innych miast. Jego zadaniem jest przebycie i sprzedanie towaru we wszystkich możliwych miastach przebywając jak najkrótsza trasę. 

# Co w tym trudnego?

Prawdopodobnie osobom, które zaznajomione są z obsługą nowoczesnych urządzeń wydaje się ten problem bardzo prosty do rozwiązania. Penie powiedzieliby, że nie jest problemem połączenie kreskami punktów na mapie, tak żeby odwiedzić je wszystkie jak najkrótszą trasą. W zależności od algorytmu wyliczania trasy problem zaczyna się przy ilości miast większej niż 9. W tym przypadku algorytm siłowy, który sprawdziłby każda możliwą kombinację potrzebowałby 8! porównań odległości miedzy miastami, żeby wybrać najkrótszą trasę. Złożoność takiego algorytmu szacowana jest na n-1, gdzie n oznacza ilość węzłów w grafie.

## Jakie rozwiązanie zaproponowaliśmy?

Z powodu złożoności obliczeniowej algorytmu siłowego zaproponowaliśmy własną wersję algorytmu zachłannego. Algorytm ten w naszej wersji liczy trasę zaczynając od pierwszego dodanego miasta i literuje od początku listy miast tyle razy ile jest miast w liście, za kazdym razem wybierając najbliżej leżące miasto o nieodwiedzonym statusie. Algorytm ten wykona n^2 porownań czyli wielokrotnie mniej niż w przypadku algorytmu zachłannego (100 vs 3628800 operacji porównań na korzyść algorytmu zachłannego). Jednakże poświęciliśmy ostateczną długość trasy, która może nie być najkrótszą z możliwych, ale zyskaliśmy oszczędność czasu gdy ją liczymy.

## Jak działa nasz algorytm?


Praca dotyczy problemu komiwojażera w skrócie TSP (travelling salesman problem). Niektórzy, którym mówiłem o problemie mówili z uśmiechem, „co za problem połączyć kreską kilka punktów? ” – Mam nadzieję, że po przeczytaniu mojej pracy już będą wiedzieli. W pracy umieściłem krótką historię, opis, kilka różnych metod poszukiwań i przykładów. Jak każda praca magisterska ta też zawiera część praktyczną. Program napisany w Javie i obecny cały czas w Internecie, czyli najbardziej rozpowszechnionym, ale niestety i najwolniejszym środowisku programistycznym. Proponuję samemu sprawdzić jak to działa. Adres strony i instrukcja obsługi znajduje się w następnych rozdziałach. Dla tych, którzy zaczną krytykować pracę lub sposób pisania w Javie powiem tylko, że jest to mój pierwszy program i na pewno nie jest on typu „Hello Word”. Do ułatwienia pisania użyłem Borland JBuilder 9 Enterprise Trial. Podobnie jak w poprzednich moich pracach w tej też pokazałem najciekawsze rozwiązania programistyczne niektórych problemów. Praca powstała w oparciu o literaturę i pisma fachowe, a przede wszystkim źródła zawarte w sieci (witryny internetowe). Kończy się wnioskami, jakie nasunęły mi się w związku z opracowaniem zebranych materiałów.