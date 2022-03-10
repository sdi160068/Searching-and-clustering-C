<!-- # Searching and clustering of vectors and timeseries in C <br><br> -->
# Αναζήτηση και Συσταδοποίηση διανυσμάτων και χρονοσειρών σε C
<!-- ## Target
The target of this project is saving vectors and timeseries via LSH and Hypercube methods , the search of nearest neighbors 
and clustering vectors via Lloyd and LSH method. -->
## Σκοπός
Σκοπός του παρόντος project είναι η αποθήκευση διανυσμάτων και χρονοσειρών με τις μεθόδους LSH και Hypercube , και κατ' επέκταση η 
εφαρμογή αναζήτησης των πλησιέστερων γειτώνων ,καθώς και η συσταδοποίηση διανυσμάτων με την μέθοδο Lloyd και LSH.

## Compile
make

<!-- ## Execute
 -->
## Εκτέλεση
Από την μεταγλώττιση (compile) των αρχείων παράγονται 2 εκτελέσιμα , το search και το cluster.
### search
./search
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
* -delta <double> , για τα grid όπου γίνεται προσαρμογή μιας χρονοσειράς
* -w \<int\> , για το w στην εξίσωση της h συνάρτησης , με τύπο:
  <br>![image](https://user-images.githubusercontent.com/81807480/157734572-1979576b-cef8-416d-87ac-72761723be12.png)<br>
όπου p είναι ένα διάνυσμα πραγματικών τιμών διάστασης d , v ένα διάνυσμα διάστασης d με συντεταγμένες από την κανονική κατανομή (γνωστή και ως γκαουσιανή κατανομή) με μέση τιμή 0 και διασπορά 1, t πραγματική τιμή από το 0 ώς το w.   
### cluster
./cluster
### Παράμετροι
* –i \<input file\>  (απαιτείται) , όπου το \<input file\> είναι παρόμοιο με αυτό του search
* –c \<configuration file\> (απαιτείται) , όπου το \<configuration file\> αποτελεί ένα αρχείο με ρυθμίσεις για το πρόγραμμα και έχει την ακόλουθη μορφή:
  <br>![image](https://user-images.githubusercontent.com/81807480/157748450-f8beee49-4993-45dc-ae54-b7bffef41274.png)<br>  
* -o \<output file\> 
* -update \<Mean Frechet or Mean Vector\> 
* –assignment \<Classic or LSH\> 
* -complete \<optional\> 
* -silhouette \<optional\>
