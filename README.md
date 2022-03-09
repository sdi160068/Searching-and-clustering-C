# Searching and clustering of vectors and timeseries in C <br><br> Αναζήτηση και Συσταδοποίηση διανυσμάτων και χρονοσειρών σε C
## Target
The target of this project is saving vectors and timeseries via LSH and Hypercube methods , the search of nearest neighbors 
and clustering vectors via Lloyd and LSH method.
## Σκοπός
Σκοπός του παρόντος project είναι η αποθήκευση διανυσμάτων και χρονοσειρών με τις μεθόδους LSH και Hypercube , και κατ' επέκταση η 
εφαρμογή αναζήτησης των πλησιέστερων γειτώνων ,καθώς και η συσταδοποίηση διανυσμάτων με την μέθοδο Lloyd και LSH.

## Compile
make

## Execute

## Εκτέλεση
Από την μεταγλώττιση (compile) των αρχείων παράγονται 2 εκτελέσιμα , το search και το cluster.
### search
./search    -algorithm \<LSH or Hypercube or Frechet\> -metric \<discrete or continuous | only for algorithm Frechet\> -delta \<double\>
### Παράμετροι
* -i \<input file\> (απαιτείται) , όπου το \<input file\> είναι ένα αρχείο κειμένου για την είσοδο του συνόλου δεδομένων (αρχείο dataset) διαχωρισμένο με στηλοθέτες (tabs), το οποίο έχει την ακόλουθη γραμμογράφηση :
  <br>![image](https://user-images.githubusercontent.com/81807480/157535032-8185dd9f-15b7-4826-8334-8641b5b3cf69.png)
* -q \<query file\> (απαιτείται) , όπου το \<query file\> είναι ένα παρόμοιο αρχείο κειμένου με το \<input file\>.
* -o \<output file\> (απαιτείται) , όπου το \<output file\> είναι το όνομα του αρχείου στο οποίο το search θα αποθηκεύσει τα αποτελέσματα.
* -k \<int\> , το πλήθος των συναρτήσεων h που χρησιμοποιούνται για τις συναρτήσεις κατακερματισμού g για το LSH. Έχει default τιμή 4. <br> 'Η για το hypercube , είναι η διάσταση στην οποία προβάλλονται τα διανύσματα. Έχει default τιμή 14.
* -L \<int\> , το πλήθος των πινάκων για το LSH. Έχει default τιμή 5.
* -M \<int\> , το μέγιστο επιτρεπόμενο πλήθος υποψηφίων διανυσμάτων που θα ελεγχθούν για το Range Search. Έχει default τιμή 10.
* -probes \<int\>, το μέγιστο επιτρεπόμενο πλήθος κορυφών του hypercube που θα ελεγχθούν. Έχει default τιμή 2.
* -algorithm , όπου δίνεται LSH ή Hypercube ή Frechet.
* -metric, όπου δίνεται discrete ή continuous (δεν έχει υλοποιηθεί).
* -delta <double>
## Περιγραφή
  
