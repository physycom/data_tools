---
documentclass: physycomit
title:  "data_tools"
author: "Fabbri, Sinigardi"
---

<a href="http://www.physycom.unibo.it"> 
<div class="image">
<img src="https://cdn.rawgit.com/physycom/templates/697b327d/logo_unibo.png" width="90" height="90" alt="© Physics of Complex Systems Laboratory - Physics and Astronomy Department - University of Bologna"> 
</div>
</a>
<a href="https://travis-ci.org/physycom/data_tools"> 
<div class="image">
<img src="https://travis-ci.org/physycom/data_tools.svg?branch=master" width="90" height="20" alt="Build Status"> 
</div>
</a>
<a href="https://ci.appveyor.com/project/cenit/data-tools"> 
<div class="image">
<img src="https://ci.appveyor.com/api/projects/status/cf9icu8pp70hqwip?svg=true" width="90" height="20" alt="Build Status"> 
</div>
</a>


`data_tools` contiene strumenti utili per calcolare e visualizzare statistiche riassuntive riguardanti dati inerziali e di geoposizionamento, ottenuti durante i test sviluppati ed eseguiti presso il Laboratorio di Fisica della Città.  

#### Synopsis
```
average.exe -period <time_interval_to_average[s]> -file filename1 -file filename2 ...
histograms [input_file.txt] OR [conf.json]
simultaneous [plt_files_basename] [input_file_1.txt] [input_file_2.txt] [input_file_3.txt] [...]
```

#### `average`
Questo tool produce un file in output avente una struttura similare a quella di input, ma con i valori di ogni riga ottenuti mediando quelli in input su un numero di righe predefinito oppure su un intervallo di tempo fissato (a seconda della scelta dell'utente). In quest'ultimo caso, si assume che il timestamp sia riportato nella prima colonna del file di input, in secondi. L'utente può definire quali colonne siano da mediare, altrimenti agisce solo sulle colonne #2, #3 and #4.
Oltre alla procedura di media, nel file di output saranno presenti tre colonne aggiuntive:
- nella prima colonna aggiuntiva, ci saranno i valori della media quadratica cumulata delle colonne analizzate, con il valor medio rimosso;
- nella seconda colonna aggiuntiva vengono riportati i numeri di righe utilizzati nella procedura di media per generare il dato presente nella riga (utile per verificare la stabilità della frequenza di dati nel caso di medie svolte su intervalli temporali, o per la controverifica del corretto funzionamento nel caso di numero fissato di righe);
- nella terza colonna aggiuntiva viene riportato il timestamp mediato per il dato.

#### `histograms`
Questo tool produce istogrammi da dati inerziali. È utile per estrarre pattern caratteristici dai dati in input (offset non documentati, sensori *freezati* che per alcuni intervalli temporali emettono sempre lo stesso dato, ecc. ecc.). Per eseguire il programma è sufficiente fornire un file di dati dalla riga di comando, ma nel caso l'utente colesse definire valori massimi e/o minimi dei plot è disponibile un file .json di configurazione tipo il seguente che può essere anch'esso passato tramite riga di comando.

Esempio di json per la configurazione:
```
{
  "input_file_name"               : "a_acc.txt",
  "output_file_histox"            : "a_acc_histox_bin.txt",
  "output_gnuplot_file_histox"    : "a_acc_histox_bin.plt",
  "output_image_file_histox"      : "a_acc_histox_bin.png",
  "output_file_histoy"            : "a_acc_histoy_bin.txt",
  "output_gnuplot_file_histoy"    : "a_acc_histoy_bin.plt",
  "output_image_file_histoy"      : "a_acc_histoy_bin.png",
  "output_gnuplot_file_histoxy"   : "a_acc_histoxy_bin.plt",
  "output_image_file_histoxy"     : "a_acc_histoxy_bin.png",
  "output_file_cart"              : "a_acc_cart_bin.txt",
  "output_gnuplot_file_cart"      : "a_acc_cart_bin.plt",
  "output_image_file_cart"        : "a_acc_cart_bin.png",
  "output_file_polar"             : "a_acc_polar_bin.txt",
  "output_gnuplot_file_polar"     : "a_acc_polar_bin.plt",
  "output_image_file_polar"       : "a_acc_polar_bin.png",
  "output_gnuplot_file_polar_exp" : "a_acc_polar_bin_exp.plt",
  "output_image_file_polar_exp"   : "a_acc_polar_bin_exp.png",
  "col_acc_x"                     :  2,
  "col_acc_y"                     :  3,
  "nbin_x"                        :  500,
  "nbin_y"                        :  500,
  "nbin_magn"                     :  500,
  "nbin_angle"                    :  62
//  "min_acc_x"                   :  0.0,
//  "max_acc_x"                   :  0.0,
//  "min_acc_y"                   :  0.0,
//  "max_acc_y"                   :  0.0,
//  "min_acc"                     :  0.0,
//  "max_acc"                     :  0.0
 }
```

#### `simultaneous`
Questo tool produce uno script gnuplot per sovrapporre dati simili provenienti da file diversi. È ideale, ad esempio, per generare plot dei test di trasferimento impulsivo di quantità di moto, plottando accelerazioni e rotazioni sui diversi assi e da differenti dispositivi nella stessa immagine. 

### Installazione
**CMake** e un compilatore compatibile con lo standard **C++11** sono necessari. Per compilare gli eseguibili è sufficiente clonare la repo e digitare i seguenti comandi:  
```
mkdir build ; cd build ; cmake .. ; cmake --build .
```
Tramite CMake è possibile generare anche i progetti per tutti gli IDE più diffusi.

`data_tools` contiene [jsoncons](https://github.com/danielaparker/jsoncons) come sottomodulo git.   
`data_tools` utilizza le librerie [boost](http://www.boost.org/).   

