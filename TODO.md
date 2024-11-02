## TODO
- controllare comportamento cursor su linux nativo

+ zoom
+ transparency
+ brush hardness and shape
+ command transform
+ draw only inside selection
+ import images

+ layers
+ filtri di convoluzione
+ visual feedback select: decorazione riquadro, flash on select



### Refactoring
- spezzare metodi init paintbrush_window
- spostare Command* in file separati
? differenziare comandi istantanei da quelli che usano il mouse
- gestire meglio scelta tool settings
? create ToolSettings struct


### Bugfixing
- corruption cmd stack quando stack size == 2 (solo linux nativo)
