## XDXF format
XDXF stands for XML Dictionary Exchange Format, and specifies a **semantic** format for storing dictionaries.

The format is **open and free** to use for everyone. Anyone interested in its further development are welcome in XDXF [Google group](https://github.com/soshial/xdxf_makedict/groups.google.com/group/xdxf-format) or [XDXF project on github](https://github.com/soshial/xdxf_makedict/).

The **main distinction of XDXF** that makes it stand out among all other dictionary formats is that it doesn't contain almost any representational information about how articles should look like.
Instead, XDXF stores only structural and semantic information in word articles.
The choice of how they have to be rendered is shifted to dictionary-browsing software ("DS"), its settings and user preferences. This might help users to be able to tweak layout, indentation, text colours, hiding examples or synonyms in order to not clutter the view etc.
Moreover, the format has many tags that are specific to dictionaries: etymologies, elaborate semantic relations, grammatical and stylistic sections and also marks, inter-article and intra-article links, categories/classes of words and many other. The format might be also useful not only for common, but also for scientific purposes. Not to mention the prolific amount of dictionary formats in use, XDXF might be a unified dictionary exchange format.
For more information on advantages of the format, consider reading the article "[Why XDXF is better?](https://github.com/soshial/xdxf_makedict/wiki/Why-is-XDXF-better%3F)". 

For opponents of using XML for storing dictionary and the problem of storing and parsing big XML-files in RAM, XDXF schema and structure of any dictionary allow to store all word articles on disk with help of hash-tables/. Some dictionary software applies this approach quite efficiently (for example, see [GoldenDict](http://goldendict.org/)).
Although, there is no software that allows editing dictionaries at the moment, XDXF is a mor eor less human-readible XML, that is quite easy to edit manually in a text editor even without prior knowledge of the format specifications.

## makedict converter
This package is for converting dictionary files many-to-many: `dictd/dsl/sdict/stardict/xdxf → dictd/stardict/xdxf`. The principal format for dictionary is XDXF though, for aforementioned reasons.

makedict helps you convert dictionary files just by such simple interface:  
* to convert from dictd format to stardict  
`makedict -o stardict file.index`  
* to convert from dictd format to xdxf format  
`makedict -o xdxf path/to/file.index`  
* to convert from dsl to stardict format  
`makedict -o stardict path/to/dictionary.dsl`  
