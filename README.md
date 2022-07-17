## XDXF format
XDXF stands for _XML Dictionary eXchange Format_. The format is **open** and **free** for everyone. 

### Advantages
* stores dictionaries without representational data in a **semantic** and **structural** way
* user may configure how word articles should look like or hide unimportant elements
* easily convertible to any other dictionary format
* human-readable, easy to fix a typo

### What is a semantic format?
The majority of formats are **non-semantic**: text formatting or tags describe how to display the content of the word article. They give no indication as to what type of content they contain or what role that content plays. Sub-meanings, word usage examples, etymology are encoded as just text with a specific formatting (bold or italic), that dictionary user cannot ever change. As a result, word articles are quite inflexible in its representation: user just cannot change anything for their liking or simply hide some unimportant parts. Also, dictionaries in these formats do not always conform to the official standard, hence while converting one has to tweak the code for every particular dictionary file.

The **main distinction** that makes XDXF stand out among all other dictionary formats is that it doesn't contain any representational information (visual cues about how article parts and text should look like). Instead, XDXF stores strictly structural and semantic information, but the choice of how word articles have to be rendered is passed to dictionary-browsing software ("DS"), its settings and user preferences. This might help users to be able to tweak layout, indentation, text colour, or hiding examples or synonyms in order to not clutter the view etc. Moreover, the format has many tags that are specific to dictionaries: etymologies, elaborate semantic relations, grammatical and stylistic sections and also marks, inter-article and intra-article links, categories/classes of words and many others. The format might be also useful not only for common, but also for scientific purposes. Not to mention the prolific amount of dictionary formats in use, XDXF might be a unified dictionary exchange format.

For more information on advantages of the format, consider reading the article "[Why XDXF is better?](https://github.com/soshial/xdxf_makedict/wiki/Why-is-XDXF-better-than-other-dictionary-formats%3F)".

### I want to create a dictionary. Where do I start?
Full format specification you may [find here](https://github.com/soshial/xdxf_makedict/blob/master/format_standard/). Anyone interested in its further development and popularization are welcome [on Github](https://github.com/soshial/xdxf_makedict/). If you need some help with converting to/from XDXF, you might ask in [XDXF Google group](https://groups.google.com/forum/#!forum/xdxf-format) or on different [converter](https://github.com/ilius/pyglossary) pages.

### Any drawbacks?
1. **XML parsing speed**. For opponents of using XML for storing dictionary and the problem of storing and parsing big XML-files in RAM, XDXF schema and structure of any dictionary allow to store all word articles on disk with help of hash-tables. Some dictionary software applies this approach quite efficiently (for example, see [GoldenDict](http://goldendict.org/)).
2. **Editing software**. Although, there is no software that allows editing dictionaries at the moment, XDXF is a human-readable XML format, that is quite easy to edit manually in a text editor even without prior knowledge of the format specifications.

### Which dictionary software supports XDXF?
* [Goldendict](https://github.com/goldendict/goldendict) (Win, Linux, MacOS)
* [Alpus](https://alpusapp.com/) (Win, Linux, MacOS, Android, iOS)
* [QTranslate](https://quest-app.appspot.com/) (Win)
* (please send me other examples, that are not listed here)

## What was `makedict`?
In the beginning of the project a converter was written to facilitate conversions to and from XDXF (`dictd/dsl/sdict/stardict/xdxf → dictd/stardict/xdxf`).

But XDXF now only supports semantic format, which essentially means that converting from visual-based format like DSL or StarDict is not possible anymore without human-written scripts that encode this formatting to logically structured data in XDXF tags. Only downgrading conversion from XDXF to visual formats now is possible automatically.

Moreover, the code written in C++ was not maintained for a long time, hence the software was **deprecated**. I suggest you use other converters like [pyglossary](https://github.com/ilius/pyglossary).
