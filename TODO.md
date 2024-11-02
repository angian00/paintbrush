## TODO
+ zoom
+ command transform

+ layers
+ brush hardness and shape

+ draw only inside selection
+ import images

+ filtri di convoluzione
+ visual feedback select: decorazione riquadro, flash on select



### Refactoring
- spezzare metodi init paintbrush_window
- spostare Command* in file separati
? differenziare comandi istantanei da quelli che usano il mouse
- gestire meglio scelta tool settings
? create ToolSettings struct


### Bugfixing

### Controllare su linux nativo
- bug corruption cmd stack quando stack size == 2 
- comportamento cursor 
