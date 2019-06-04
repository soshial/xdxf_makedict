## XDXF format
XDXF stands for XML Dictionary Exchange Format, and specifies a **semantic** format for storing dictionaries. Full format specification you may find inside [`format_standard`](https://github.com/soshial/xdxf_makedict/blob/master/format_standard/xdxf_description.md) folder.

The format is **open and free** to use for everyone. Anyone interested in its further development and popularization are welcome [on Github](https://github.com/soshial/xdxf_makedict/). If you need some help with converting to/from XDXF, you might ask in [XDXF Google group](https://groups.google.com/forum/#!forum/xdxf-format) or on different [converter](https://github.com/ilius/pyglossary) pages.

### Advantages
The **main distinction** that makes XDXF stand out among all other dictionary formats is that it doesn't contain almost any representational information about how articles should look like.
Instead, XDXF stores only structural and semantic information.
The choice of how word articles have to be rendered is passed to dictionary-browsing software ("DS"), its settings and user preferences. This might help users to be able to tweak layout, indentation, text colours, hiding examples or synonyms in order to not clutter the view etc.
Moreover, the format has many tags that are specific to dictionaries: etymologies, elaborate semantic relations, grammatical and stylistic sections and also marks, inter-article and intra-article links, categories/classes of words and many other. The format might be also useful not only for common, but also for scientific purposes. Not to mention the prolific amount of dictionary formats in use, XDXF might be a unified dictionary exchange format. For more information on advantages of the format, consider reading the article "[Why XDXF is better?](https://github.com/soshial/xdxf_makedict/wiki/Why-is-XDXF-better-than-other-dictionary-formats%3F)".

### Any drawbacks?
1. **XML parsing speed**. For opponents of using XML for storing dictionary and the problem of storing and parsing big XML-files in RAM, XDXF schema and structure of any dictionary allow to store all word articles on disk with help of hash-tables/. Some dictionary software applies this approach quite efficiently (for example, see [GoldenDict](http://goldendict.org/)).
2. **Editor software**. Although, there is no software that allows editing dictionaries at the moment, XDXF is a more or less human-readible XML, that is quite easy to edit manually in a text editor even without prior knowledge of the format specifications.

### Which dictionary software supports XDXF?
* [Goldendict](https://github.com/goldendict/goldendict) (Win, Linux, MacOS, Android)
* (please send me other examples, that I dont know of)

## What was `makedict`?
In the beginning of the project a converter was written to facilitate conversions to and from XDXF (`dictd/dsl/sdict/stardict/xdxf → dictd/stardict/xdxf`).

But XDXF now only supports semantic format, which essentially means that converting from visual-based format like DSL or StarDict is not possible anymore without human-written scripts that encode this formatting to logically structured data in XDXF tags. Only downgrading conversion from XDXF to visual formats now is possible automatically.

Moreover, the code written in C++ was not maintained for a long time, hence the software was **deprecated**. I suggest you use other converters like [pyglossary](https://github.com/ilius/pyglossary).
