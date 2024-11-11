## TODO

- zoom
    - centrare zoom su pos

- aggiungere status bar con pos cursore e zoom level


- moveView on space

+ command transform

+ layers
+ brush hardness and shape

+ draw only inside selection
+ import images

+ filtri di convoluzione
+ visual feedback select: decorazione riquadro, flash on select



### Refactoring
- spostare PaintbrushScrollArea::onViewMoved in canvas (o vieceversa)

- spezzare metodi init paintbrush_window
- spostare Command* in file separati
? differenziare comandi istantanei da quelli che usano il mouse
- gestire meglio scelta tool settings
? create ToolSettings struct


### Bugfixing

### Performance
- lento quando la scala e' elevata (== quando la canvas e' grande)

### Controllare su linux nativo
- comportamento cursor 
