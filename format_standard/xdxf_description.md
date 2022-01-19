                                    XDXF standard;  Draft 034;  19 January 2022
## Introduction
XDXF stands for XML Dictionary Exchange Format, and specifies a **semantic** format for storing dictionaries.

The format is **open and free** to use for everyone. Anyone interested in its further development and popularization are welcome [on Github](https://github.com/soshial/xdxf_makedict/). If you need some help with converting to/from XDXF, you might ask in [XDXF Google group](https://groups.google.com/forum/#!forum/xdxf-format) or on different [converter](https://github.com/ilius/pyglossary) pages.

The **main distinction of XDXF** that makes it stand out among all other dictionary formats is that it doesn't contain almost any representational information about how articles should look like.
Instead, XDXF stores only structural and semantic information in word articles.
The choice of how they have to be rendered is shifted to dictionary-browsing software ("DS"), its settings and user preferences. This might help users to be able to tweak layout, indentation, text colours, hiding examples or synonyms in order to not clutter the view etc.
Moreover, the format has many tags that are specific to dictionaries: etymologies, elaborate semantic relations, grammatical and stylistic sections and also marks, inter-article and intra-article links, categories/classes of words and many others. The format might be also useful not only for common, but also for scientific purposes. Not to mention the prolific amount of dictionary formats in use, XDXF might be a unified dictionary exchange format.
For more information on advantages of the format, consider reading the article "[Why XDXF is better?](https://github.com/soshial/xdxf_makedict/wiki/Why-is-XDXF-better%3F)". 

For opponents of using XML for storing dictionary and the problem of storing and parsing big XML-files in RAM, XDXF schema and structure of any dictionary allow to store all word articles on disk with help of hash-tables/. Some dictionary software applies this approach quite efficiently (for example, see [GoldenDict](http://goldendict.org/)).
Although, there is no software that allows editing dictionaries at the moment, XDXF is a more or less human-readable XML, that is quite easy to edit manually in a text editor even without prior knowledge of the format specifications.

### Changelog (rev. 34) 2022-01-20
* since rev. 34 the format is only semantic and cannot store any presentational or visual data
* the language code limitation is removed: all languages that exist in BCP47 standard are supported (use http://schneegans.de/lv/?tags=hy-Latn-IT-arevela for validation)
* multilingual dictionaries are now supported: a dictionary may have multiple languages, that are translated from and into. It is also allowed to mark `<k>` and `<def>` tags with `xml:lang`
* description supports line breaks
* transcription info can be directly inside `def` tag

### Changelog (rev. 33)
* `<deftext>` introduced in order to fix multiple errors in DTD scheme
* `<rref>` tag: added `lctn` and `type` attributes, links are not stored inside the tag anymore
* `<kref>` tag: `idref` attribute introduced
* `<c>` tag: added necessary hash sign # in attribute
* `<categ>` tag now is a list of `<kref>` tags
* `<etm>` may now have `<mrkd>` inside to mark etymological ancestors/cognates
* `<dtrn>` may now contain `<kref>` tag(s) inside
* `<u>` tag for underlined text introduced
* `<br/>` tag introduced for newlines inside articles
* `<ex>` now might have `<iref>` tag inside
* `<ex>`, `<tr>`, `<co>` tags now may have user-set attribute values

#### Known limitations:
* Many dictionary creators wished that XDXF supports some specific grammar forms of their language. Unfortunately, all possible grammar forms for all possible
languages cannot be formalised in a concise format such as XDXF. Also, supporting tables with grammar forms might over-sophisticate the format. Therefore, we
resorted to plain-text grammar information.

## Format description
### File structure
Each dictionary is located in its own folder, the name of the folder is used as ID, and
must contain only Latin characters and must not contain spaces or other special characters.
So, if the dictionary name is "Webster's Unabridged Dictionary published in 1913" then
the folder name should be something like "Webster1913". The dictionary file itself is
always "dict.xdxf". It is recommended for each dictionary to have a set of icons for toolbars
and a large icon for the front page. The sizes should be of size like: 16x16, 32x32, 512x512. And the
filenames would be icon16.png icon32.png and icon512.png respectively.
Note that all file names are case-sensitive.

All XDXF dictionary text files (those with .xdxf extension) are in XML format with any Unicode encoding (usually UTF-8).
Any non-Unicode encodings are strictly prohibited.

### Tags and structure:

`<xdxf revision="DD">` is the **root element**. The`revision` attribute specifies an exact version of the format standard of an XDXF file. Obligatory.

The structure of a file is divided into 2 parts: the ``<meta_info>`` and the `<lexicon>`:
```xml
<xdxf revision="34">
    <meta_info>
        All meta information about the dictionary: its title, author etc. 
    </meta_info>
    <lexicon>
        <ar>article 1</ar>
        <ar>article 2</ar>
        <ar>article 3</ar>
        <ar>article 4</ar>
        ...
    </lexicon>
</xdxf>
```

1. `<meta_info>` is the container for all meta information about the dictionary. It contains:
    1. `<title>` The short title of the dictionary (so that it can fit small screens and look well in lists). Examples: "Oxford English-Arabic" or "Oxford Picture (En-Ru)"
    2. `<full_title>` Full name of the dictionary, like it would appear on the book cover. Example: "Oxford Advanced Learners Dictionary 8th Edition" or "Oxford Picture Dictionary English-Russian: Bilingual Dictionary for Russian-speaking teenage and adult students of English (Oxford Picture Dictionary 2E)"
        *Tip*: It can contain titles in several languages or several titles at once.
    3. `<publisher>` The official publisher of the dictionary; optional.
    4. `<authors>` contains a list of `<author>` tags and represents all people (organizations) that took part in making dictionary: lexicographers, proofreaders, programmers etc. Optional.
        * `<author role="xxx">` One tag for each author.

        *Tip*: some authors might have >1 roles: in this case s/he should be listed using several `<author>` tags
    5. `<description>` of the dictionary is in free words or an annotation/resume/reader's note from the publisher.  
        It is recommended to include the following: Copyright, License, whence this file can be downloaded, whence the unformatted file (i.e. the original dictionary file before the conversion into XDXF format) can be downloaded, whence the original unformatted dictionary file was obtained, and link to the script which was used to convert the original unformatted dictionary file into XDXF. The description may contain XHTML tags that are allowed in XDXF (specified below).
    6. `<abbreviations>` section contains a list of `<abbr_def>` tags. It contains all abbreviations used for labels in the dictionary
        (grammar labels, for example). `<abbr_def>` tag defines an abbreviation and contains two different tags:
        * `<abbr_k>` (stands for abbreviation key): abbreviated text.
        * `<abbr_v>` (stands for value): full text, abbreviation description that will show up when you hover over it.
        
        `<abbr_def>` might contain a `type` attribute, which states which type of label this abbreviation is: 
        * `<abbr_def type="grm">` — stating grammatical features of word (noun, past participle etc.)
        * `<abbr_def type="stl">` — stylistic properties of a word (vulgar, archaic, obsolete, poetic, disapproving etc.)
        * `<abbr_def type="knl">` — area/field of knowledge (computers, literature, culinary, typography etc.)
        * `<abbr_def type="aux">` — simple subsidiary words like ('e.g.', 'i.e.', 'cf.', 'also', 'rare' etc.).
        * `<abbr_def type="oth">` — others  

        *Notice!* Note that each abbreviation definition may contain more than one `<abbr_k>` per `<abbr_def>` to specify synonyms like "Ave." and "Av.", but `<abbr_v>` tag can be only one.
        ```xml
        <abbreviations>
            <abbr_def type="grm"><abbr_k>n.</abbr_k><abbr_v>noun</abbr_v></abbr_def>
            <abbr_def type="knl"><abbr_k>polit.</abbr_k><abbr_v>politics</abbr_v></abbr_def>
            <abbr_def><abbr_k>Av.</abbr_k><abbr_k>Ave.</abbr_k><abbr_v>Avenue</abbr_v></abbr_def>
        </abbreviations>
        ```
    7. `<file_ver>`, `<creation_date>` are obligatory info.
        `<last_edited_date>`, `<dict_edition>`, `<publishing_date>`, `<dict_src_url>` are optional meta info.
        All dates should be formatted as `DD-MM-YYYY`. When the date is not fully known, there should be zeros: 05-00-2011.
    8. `languages` tag lists which languages are used in this dictionary. `from` sub-tags list languages that key-phrases (`<k>`) represent. `to` sub-tags list languages that definitions (`<def>`) represent.

2. `<lexicon>` is the container for all `<ar>` (article) tags. The `<ar>` tag groups together all the stuff related to one key-phrase.  
    
    The following tags are allowed inside `<ar></ar>` tag.  
    1. `<k>` A "key phrase", that is written inside `<k>` tag is a sequence of letters/ideograms which a correspondent word article is identified with.
        Each article may contain more than one key phrase, but always at least one. If there are more than one `<k>`, DS must display all key-phrases. that are assigned to the article, no matter which key-phrase a user was looking for. Developers of DS should make sure that convenient search through all keys is possible (with disregarding diacritics, wildcards and so on).
        *Notice*, that if it's not possible to merge two articles into one article because of key phrases, then there could be several different articles with identical key-phrases (but different article bodies). For example:
        
        ```xml
        <lexicon>
            <ar>
                <k>disc</k>
                <k>disk</k>
                <def cmt="a device for storing information">
                    <deftext>......</deftext>
                </def>
            </ar>
            <ar>
                <k>disc</k>
                <def cmt="round object">
                    <deftext>......</deftext>
                </def>
            </ar>
        </lexicon>
        ```
        Also, `<sup>` and `<sub>` tags are allowed inside `<k>`.
        * `<opt>` Marks optional part of a key-phrase. Articles are searched by the `<k>` contents without `<opt>` contents, but are showed in the article with it. Tag `<opt>` might be used only inside `<k>` tag.
    2. `<def>` This tag contains either whole body of a word article inside `<ar>`, or a definition itself or a group of definitions, that fall into a certain category.  
        *Tip*. Those categories could be different parts of speech or have different etymology, for example, or the same sense with different connotations.  
        Note that `<def>` tags can be nested inside each other and they usually do, unless the article is as plain as "1 word-to-1 word" translation. Check out some examples [here](https://github.com/soshial/xdxf_makedict/issues/37).  
        Main `<def>` tag must be inside `<ar>` even if an article is simple and there is nothing to group. `<deftext>` tags contain actual textual definitions.  
        \* They might have a `cmt` (comment) attribute, that helps disambiguate one definition from others.  
        \* They might have a unique lowercase alphanumerical `id` attribute [01-9a-z], that can be referred to from another article.  
        \* They might have an integer/float attribute `freq`: some absolute or relative frequency value of the definition.
        
        The DS should distinguish visually one definition from another according to nesting level by means of indentation, font size or enumeration definitions with  '1)','2)'... or 1.','2.'... or 'A.','B.'... etc. Consider checking out the examples or the DTD schema to understand the structure better.
        1. `<gr>` Specifies grammar information about the word. Might contain different word forms, word usage, grammatical labels and other information of this sort.
        2. `<tr>` Marks transcription/pronunciation information; IPA symbols are the default.
            Might also have "format" attribute with values "X-SAMPA" or "erkIPA".
        3. `<kref>` is a reference to some `<ar>` or `<def>`, that is located in the same dictionary file. Links should be clickable. Examples:
            * Code `<kref>answer</kref>` will link to the article(s), which has `<k>answer</k>`.
            * If `<kref>` has `idref` attribute, then it references the `<def>` tag that has an `id` equal to this attribute. The word "answered" in code `<kref idref="nf0837glo9">answered</kref>` will link to the verb "answer" (and not the noun because of the specific `id`).
            
            For the additional attributes and usage, see `<sr>`.
        4. `<dtrn>` This tag marks *Direct Translation* of the key-phrase (not usually used for explanatory dictionaries). But in non-monolingual dictionaries the tag should be used to help dictionary software automatically extract main and simplest translations of the key-phrase — this might be useful for:
            * automatic extraction of data for tooltip translations (e.g. [like qDictionary](http://www.cgliberty.com/articles/software/dictionaries/translite.jpg))
            * make visible `<dtrn>` in the word-list to avoid too frequent "full article" look-ups ([example](https://github.com/soshial/xdxf_makedict/wiki/Why-is-XDXF-better%3F))  
            
            *Tip*. These words should be marked out for easier looking through the article (usually these words are displayed as bold, also sometimes in dark-orange). These words might also be automatically look as links (`<kref>`) leading to the article of the opposite language pair (if they are present in that second dictionary).
        5. `<rref>` Reference to a resource file (audio, video, gif, image files), which should be located in the same folder or any subfolder as the dictionary file is.
            * `lctn` (physical address of the resource) is put inside `<rref>` tag.
            * `type` attribute specifies the MIME-type of the resource file.
            * optional `start` and `size` attributes are necessary for audio and video files, when the reference points to a certain part of a file. It is very convenient to keep all audio data for the dictionary in one file, referring to different parts of it from articles. The attribute `start` specifies an offset: a position in the file of the first byte of the chunk of interest, and `size` specifies its length in bytes. If the "start" attribute is omitted then it is assumed that it is 0. If the `size` attribute is omitted then it is assumed that the file is to be played up to the end.  
            
            Usage: `<rref lctn="audio.ogg" type="audio/opus" start="xxx" size="xxx">crawl</rref>` or just `<rref lctn="audio/transcription1.mp3"/>`
        6. `<iref href="http://www.somewebsite.com">description</iref>` Reference to an external online resource. Preserving the protocol ("<http://>", "<https://>" etc.) is obligatory.
        7. `<abbr>` tag marks an abbreviated label. Note, that encountered labels should have explanation listed in correspondent `<abbr_k>` tag in `<abbreviations>` section.
        8. `<c c="#xxxxxx">...</c>` ("xxxxxx" stands for a 6-digit hex color code). Marks text with a given color.
            The syntax for "c" attribute is the same as for "color" attribute of "font" tag in HTML.
            If the color attribute is omitted, the default color is implied. The default color is chosen by DS.
        9. `<ex>` Marks the text of an example (usually shown in grey or other color by DS).  
            *Tip* (indexing): examples should be indexed by default, but users should be able to configure, whether they want examples to be indexed and searched for or not.
            Attribute `type` might be:
            * `exm` - common examples with or without translations
            * `phr` - might contain any type of phrasemes (idioms, collocations, clichés etc.)
            * `prv` - proverbs
            * `oth` - other
            
            Attributes `source` and `author` specify where the example was taken from.
        
            An `<ex>` tag might contain these tags:
            * `<ex_orig>` is for the original phrase of the example (amount: 1 or more).  
            * `<ex_tran>` is optional; may be multiple translations (amount: 0 or more).  
            * Inside the previous two there might be useful `<mrkd>` tags. They are used to mark down main word(s) of an article both in original phrase and in translation ([example from Wiktionary](https://github.com/soshial/xdxf_makedict/raw/master/format_standard/images/mrkd_tag_in_examples.png)).
            * `<iref>` may contain a link to external resource.
        10. `<co>` Marks the text of an editorial comment that elucidates meaning or context (shown in a different colour by program depending on `type`). `type` attribute specifies what kind of comment it is: grammatical, stylistic, usage etc., anything that didn't fit for be placed into corresponding sections.   
            *Tip* (indexing): comments are normally indexed.
        11. `<sr>` is a section dedicated to semantic relations to other words like synonyms, holonyms, hypernyms etc.
            It uses `<kref>` with `type` attribute to relate to other words and definitions. Possible `type` values are:
            * `syn` and `ant` — synonyms and antonyms
            * `hpr` and `hpn` — hyperonyms and hyponims (these incorporate [troponyms](http://en.wikipedia.org/wiki/Troponymy))
            * `par` and `spv` — paronyms and spelling variants
            * `mer` and `hol` — meronyms and holonyms
            * `ent` — entailment, special verb category: the v. Y is entailed by X if by doing X you must be doing Y
                might also be used for nouns: doing, having or being this noun entails some other thing
                (doing a "felony" entails being a "criminal")
            * `rel` — denotes relevance (for ex., "handsome" and "ugly" are relevant to "appearance")  
            
            `kcmt` attribute is used to specify to what extent this word is related to ours or to specify additional information about the word (like gender or case).
            This is how `<sr>` section would look like for "traveller":
            ```xml
            <sr>
                <kref type="spv" idref="35fo86fdr">traveler</kref>
                <kref type="rel" kcmt="partly">tourist</kref>
                <kref type="syn" idref="pu4o202oi"><abbr>obsol.</abbr>voyager</kref>
            </sr>
            ```
        12. `<etm>` Etymological information about the word. `<etm>` may have `<mrkd>` tag inside to mark etymological ancestors or cognates of the key-word.
        13. `<di>` Marks the part of `<def>`'s text that should not be indexed. Might be used only inside `<def>` and some its children: `<co>`, `<ex>`, `<etm>`, `<phr>`
        14. `<categ>` tag is somewhat an equivalent of Wiktionary categories (e.g. 'inquisition' might have 'Medieval history' category or 'checkers' having 'Pluralia tantum' category). All categories are simply `<kref>` tags with `idref` attributes, that link to pre-created articles with some description. DS must create a list of all articles that link to a corresponding category.
        15. All style tags `<sup>`, `<sub>`, `<i>`, `<b>`, `<u>` are used for representation of text. `<br/>` tag is used for newlines. Any other newline spaces besides this tag are ignored.


## Examples:
For examples see https://github.com/soshial/xdxf_makedict/master/sample-dicts/.
