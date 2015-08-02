                                    XDXF standard;  Draft 032;  09 June 2013

## Description
XDXF stands for XML Dictionary Exchange Format, and as its name implies, specifies a format for dictionary data files.
Some dictionary software supports XDXF natively (see for ex., GoldenDict, XDClient).

Each dictionary is located in its own folder, the name of the folder is used as ID, and
must contain only Latin characters and must not contain spaces or other special characters.
So, if the dictionary name is "Webster's Unabridged Dictionary published in 1913" then
the folder name should be something like "Webster1913".  The dictionary file itself is
always "dict.xdxf". It is recommended for each dictionary to have a set of icons for toolbars
and a large icon for the front page. The sizes should be: 16x16, 32x32, 512x512.  And the
filenames would be icon16.png icon32.png and icon512.png respectively.
Note that all file names are case sensitive.

All XDXF dictionary text files (those with .xdxf extension) are in XML format with any Unicode encoding (usually UTF-8).
Any other encodings are strictly prohibited.

### Changelog (rev.32):

* markdown is now displayed correctly
* `<phr>` tag removed as it was already present in <ex>
* `<abr_def>` tag: attribute `type` introduced
* `<xdxf>` has new obligatory attribute: the `revision` (version) of the format standard the file adheres to
* visual format is now considered as not recommended
* `<file_ver>`, `<creation_date>` were made obligatory meta-info
* examples now adhere to the format specs (thanks to @lastivka)
* `<categ>` tag introduced
* `<ex>` tag `type` attribute might have value `oth` now; `mrkd` tag introduced; attributes `source` and `author` introduced
* `<kref>` tag: `kcmt` attribute introduced
* `<def>` tag: `id` attribute is case-sensitive (as in HTML)

## XDXF Tags:

`<xdxf lang_from="XXX" lang_to="XXX" format="FORMAT" revision="DD">` is the **root element**. It must have 4 attributes:
* `lang_from` and `lang_to` values are 3-letter language codes from [ISO 639-3 standard](http://sil.org/iso639-3/)
    and represents the language of key-phrases and definitions respectively.
* The `format` attribute specifies the default formatting for the dictionary and might be either `visual` or
    `logical`. The default format might be overwritten for specific articles as described below.
    * In visual format, the articles are formatted visually and are intended to be shown by
        dictionary programs (referred as DP) as is without inserting or removing any spaces or EOLs.
        However, DPs may mark the content of some logical tags (like `<gr>` or `<abbr>`) with different colors.  
        **NB**! Remember, that visual format is NOT recommended! XDXF is developed especially for logically structured
        dicts and the visual format was introduced only to be compatible with dicts converted from old plain-text formats.
    * In logical format, the articles are not formatted visually and shells are responsible
        for formating them before presenting them to the user.
* `revision` attribute specifies format version that your XDXF file is formatted in.  
    *Tip*: this attribute is obligatory, but it was first introduced in recent format standard, so it might be
    absent in some old xdxf files.

The structure of a file is divided into 2 parts: the ``<meta_info>`` and the `<lexicon>`:
```xml
<xdxf ...>
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
    1. `<title>` The short title of the dictionary written in English
    2. `<full_title>` Full name of the dictionary, like it would appear on the book cover.  
        *Tip*: It can contain non-English title.
    3. `<publisher>` The official publisher of the dictionary; optional.
    4. `<authors>` contains a list of `<author>` tags and represents all people (organizations) that took part in making dictionary: lexicographers, proofreaders, programmers etc. Optional.
        * `<author role="xxx">` One tag for each author.

        *Tip*: some authors might have >1 roles: in this case s/he should be listed using two `<author>` tags
    5. `<description>` of the dictionary is in free words or an annotation/resume/reader's note from the publisher.  
        It is recommended to include the following: Copyright, License, whence this file can be downloaded, whence the unformatted file (i.e. the original dictionary file before the conversion into XDXF format) can be downloaded, whence the original unformatted dictionary file was obtained, and link to the script which was used to convert the original unformatted dictionary file into XDXF. The description may contain XHTML tags that are allowed in XDXF (specified below).
    6. `<abbreviations>` section contains a list of `<abbr_def>` tags. It contains all abbreviations used for labels in the dictionary
        (grammar labels, for example). `<abbr_def>` tag defines an abbreviation and contains two different tags:
        * `<abbr_k>` (stands for abbreviation key): abbreviated text.
        * `<abbr_v>` (stands for value): full text, abbreviation description that will show up when you hover over it.
        
        `<abbr_def>` might contain a `type` attribute, which states which type of label this abbreviation is: 
        * `<abbr_def type="grm">` — stating grammatical features of word (noun, past participle etc.)
        * `<abbr_def type="stl">` — stylistical properties of a word (vulgar, archaic, poetic etc.)
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

2. `<lexicon>` is the container for all `<ar>` (article) tags. The `<ar>` tag groups together all the stuff related to one key-phrase.  
    
    They can have an optional attribute `f`, eg. `<ar f="x">` which may have value either `v` (visual) or `l` (logic) and
    can be used to override locally the default dictionary format, which was specified in `<xdxf>` tag.  
    **NB**! Consider, that using visual format is NOT recommended and its support by PDs is unlikely!  
    
    The following 2 tags are allowed only in-between `<ar></ar>` tags.  
    1. `<k>` Key phrase is a unique sequence of letters/ideograms by which correspondent article is identified and could be found.
        Article may contain more than one key phrase, but always at least one.  
        *Notice!* If there are more than one `<k>`, the DP should display all variants of key-phrase of the article. Tag `<k>` may not be nested in another `<k>`.
        * `<opt>` Marks optional part of key-phrase. The article is searched by the `<k>` contents without `<opt>` contents, but showed in the article with it. Tag `<opt>` might be used only in-between `<k></k>` tags.
    2. `<def>` This tag marks the whole body of word article, each definition itself and a group of definitions which fall into a certain category.  
        *Tip*. Those categories could be different parts of speech or have different etymology, for example, or the same sense with different connotations.    
        Note that `<def>` tags can nest and they usually do, unless the article is plain as "1 word-to-1 word" translation.  
        The `<def>` tag must be inside `<ar>` even if the article is simple and there is nothing to group.  
        \* They might have a `cmt` (comment) attribute, that helps disambiguate one definition from others.  
        \* They might have a unique lowercase alphanumerical case-sensitive `id` attribute [01-9a-zA-Z], that can be referred to from another article.  
        \* They might have an integer/float attribute `freq`: some absolute or relative frequency value of the definition.
        
        In articles with visual format `<def>` tags do not effect the formatting.
        For articles that have logical format DPs must distinguish visually one definition from another according to the nesting level by means of indentation, lessening font size or numerating definitions with  '1)','2)'... or 1.','2.'... or 'A.','B.'... etc. before each definition.
        1. `<gr>` Specifies grammar information about the word. Might contain different word forms, word usage, grammatical labels and other information of this sort.
        2. `<tr>` Marks transcription/pronunciation information; IPA symbols are the default.
            Might also have "mode" attribute with values "X-SAMPA" or "erkIPA".
        3. `<kref>` Simple reference to another key-phrase, which is located in the same file. For the additional attibutes, see `<sr>`
        4. `<dtrn>` This tag marks *Direct Translation* of the key-phrase (not usually used for explanatory dictionaries). But in non-monolingual dictionaries the tag should be used to help dictionary software automatically extract main and simplest translations of the key-phrase — this might be useful for:
            * automatic extraction of data for tooltip translations (e.g. [like qDictionary](http://www.cgliberty.com/articles/software/dictionaries/translite.jpg))
            * make visible `<dtrn>` in the word-list to avoid too frequent "full article" look-ups ([example](http://img703.imageshack.us/img703/5796/snapshot13s.png))  
            
            *Tip*. These words should be marked out for easier looking through the article (usually these words are displayed as bold, also sometimes in dark-orange). These words might also be automatically look as links (`<kref>`) leading to the article of the opposite language pair (if they are present in that second dictionary).
        5. `<rref>` Reference to an audio (resource) file (mp3, ogg or opus format), which should be located in the same folder as dictionary file.
            * optional `start` and `size` attributes are necessary for audio and video files, when the reference points to a certain part of a file. It is very convenient to keep all audio data for the dictionary in one file, referring to different parts of it from articles. The attribute `start` specifies an offset: a position in the file of the first byte of the chunk of interest, and `size` specifies its length in bytes. If the "start" attribute is omitted then it is assumed that it is 0. If the `size` attribute is omitted then it is assumed that the file is to be played up to the end.  
            
            Usage: `<rref start="xxx" size="xxx">crawl</rref>` or `<rref start="xxx" size="xxx"/>`
        6. `<iref href="http://www.somewebsite.com">` Reference to an Internet resource. Preserving the prefix ("http://", "https://") is obligatory.
        7. `<abbr>` tag marks an abbreviated label. Note, that encountered labels should have explanation listed in correspondent `<abbr_k>` tag in `<abbreviations>` section.
        8. `<c c="xxxxxx">...</c>` (c c stands for 6-digit hex color code) Marks text with a given color.
            The syntax for "c" attribute is the same as for "color" attribute of "font" tag in HTML.
            If the color attribute is omitted, the default color is implied. The default color is chosen by the DP.
        9. `<ex>` Marks the text of an example (usually shown in grey or other color by the DP).  
            *Tip* (indexing): examples must be indexed by default, but users should be able to configure whether they want examples to be indexed and searched for or not.  
            Attribute `type` might be:
            * `exm` - usual examples with or without translations
            * `phr` - might contain any type of phrasemes (idioms, collocations, clichés etc.)
            * `prv` - proberbs
            * `oth` - other
            
            Attributes `source` and `author` specify where the example was taken from.
        
            The tag should contains tags:
            `<ex_orig>` is for the original phrase of the example (amount: 1 or more).  
            `<ex_tran>` is optional; may be multiple translations (amount: 0 or more).  
            `<mrkd>` is an optional tag, that is used to mark down the target word in the original phrase and a translation ([example from Wiktionary](http://img834.imageshack.us/img834/634/snapshot14t.png)).
        10. `<co>` Marks the text of an editorial comment that elucidates the meaning or context (shown in a different color by the program; usually grey).  
            *Tip* (indexing): comments are normally indexed.
        11. `<sr>` is a section dedicated to sematic relations to other words like synonyms, holonyms, hypernyms etc.
            It uses `<kref>` to address other word with additional tag attributes. `<sr>` section looks like:
```xml
<sr>
    <kref type="syn" kcmt="obsolete">game</kref>
    <kref type="hol" kcmt="partly">play</kref>
</sr>
```
            
            Possible `type` values:
            * `syn` and `ant` — synonyms and antonyms
            * `hpr` and `hpn` — hyperonyms and hyponims (these incorporate [troponyms](http://en.wikipedia.org/wiki/Troponymy))
            * `par` and `spv` — paronyms and spelling variants
            * `mer` and `hol` — meronyms and holonyms
            * `ent` — entailment, special verb category: the v. Y is entailed by X if by doing X you must be doing Y
                might also be used for nouns: doing, having or being this noun entails some other thing
                (doing a "felony" entails being a "criminal")
            * `rel` — denotes relevance (for ex., "handsome" and "ugly" are relevant to "appearance")  
            
            `kcmt` attribute is used to specify to what extent this word is related to ours or to specify additional information about the word (like gender or case) 
        12. `<etm>` Etymological information about the word.
        13. `<di>` Marks the part of `<def>`'s text that should not be indexed. Might be used only inside `<def>` and some its children: `<co>`, `<ex>`, `<etm>`, `<phr>`
        14. `<categ>` tag is somewhat an equivalent of Wiktionary categories (e.g. 'inquisition' might have a category 'Medieval history' or 'boxers' having 'Pluralia tantum')

### Non-XDXF Tags:

For visually formatted articles in addition to XDXF tags, the following XHTML tags are allowed:
  `<sup>`, `<sub>`, `<i>`, `<b>`, `<big>`, `<small>`, `<blockquote>`
Their syntax and semantics are the same as in XHTML.


## Examples:

### Visual format
**NOT RECOMMENDED**, only for compatibility with old converted dictionaries.
```xml
<?xml version="1.0" encoding="UTF-8" ?>
<!DOCTYPE xdxf SYSTEM "https://raw.github.com/soshial/xdxf_makedict/master/format_standard/xdxf_strict.dtd">
<xdxf lang_from="ENG" lang_to="ENG" format="visual" revision="032beta">
    <meta_info>
        <title>Webster's Dictionary</title>
        <full_title>Webster's Unabridged Dictionary</full_title>
        <description>Webster's Unabridged Dictionary published 1913 by the... </description>
        <abbreviations>
            <abbr_def><abbr_k>n.</abbr_k> <abbr_v>noun</abbr_v></abbr_def>
            <abbr_def><abbr_k>v.</abbr_k> <abbr_v>verb</abbr_v></abbr_def>
            <abbr_def><abbr_k>Av.</abbr_k><abbr_k>Ave.</abbr_k><abbr_v>Avenue</abbr_v> </abbr_def>
        </abbreviations>
        <file_ver>001</file_version>
        <creation_date>07-04-2013</creation_date>
    </meta_info>
    <lexicon>
        <ar>
            <k><opt>The </opt>United States<opt> of America</opt></k>
            Соединенные Штаты Америки
        </ar>
        <ar f="l">
            <k>record</k>
            <def>
                <def>
                    <gr><abbr>n.</abbr></gr>
                    [<tr>re'kord</tr>]
                    Anything written down and preserved.
                </def>
                <def>
                    <gr><abbr>v.</abbr></gr>
                    [<tr>reko'rd</tr>]
                    To write down for future use.
                </def>
            </def>
        </ar>
        <ar>
            <k>home</k>
            <def>
                [<tr>ho:um</tr>]
                <gr><abbr>n.</abbr></gr>
                <rref start="16384" size="512"> sounds_of_words.ogg </rref>
                1) One's own dwelling place; the house in which one lives.
                2) One's native land; the place or country in which one dwells.
                3) The abiding place of the affections. <ex>For without hearts there is no home.</ex>
                4) <dtrn>дом</dtrn> at home - дома, у себя; make yourself at home - будьте как дома
                <ex>XDXF <iref href="http://xdxf.sourceforge.net"><b>Home</b> page</iref></ex>
                See also: <kref>home-made</kref>
            </def>
        </ar>
    </lexicon>
</xdxf>
```
### Logical format
Example of the RECCOMENDED logical format:
```xml
<?xml version="1.0" encoding="UTF-8" ?>
<!DOCTYPE xdxf SYSTEM "https://raw.github.com/soshial/xdxf_makedict/master/format_standard/xdxf_strict.dtd">
<xdxf lang_from="ENG" lang_to="ENG" format="logical" revision="032beta">
    <meta_info>
        <title>Webster's Dictionary</title>
        <full_title>Webster's Unabridged Dictionary</full_title>
        <description>Webster's Unabridged Dictionary published 1913 by the Webster Institute</description>
        <abbreviations>
          <abbr_def><abbr_k>n.</abbr_k> <abbr_v>noun</abbr_v></abbr_def>
          <abbr_def><abbr_k>v.</abbr_k> <abbr_v>verb</abbr_v></abbr_def>
          <abbr_def><abbr_k>Av.</abbr_k><abbr_k>Ave.</abbr_k><abbr_v>Avenue</abbr_v> </abbr_def>
        </abbreviations>
        <file_ver>001</file_version>
        <creation_date>07-04-2013</creation_date>
    </meta_info>
    <lexicon>
        <ar>
            <k>home</k>
            <def>
                <tr>'həum</tr>
                <gr><abbr>n.</abbr></gr><rref start="16384" size="512">sounds_of_words.ogg</rref>
                <def>One's own dwelling place; the house in which one lives.</def>
                <def>One's native land; the place or country in which one dwells.</def>
                <def>
                    The abiding place of the affections.
                    <ex>For without hearts there is no home.</ex>
                </def>
                <def>
                    <dtrn>дом</dtrn>, at home - дома, у себя;
                    <ex>
                        <ex_orig>make yourself at <mrkd>home</mrkd></ex_orig>
                        <ex_tran>будьте как <mrkd>дома</mrkd></ex_tran>
                    </ex>
                    <categ>Society</categ>
                </def>
                <co>XDXF <iref href="http://xdxf.sourceforge.net"><b>Home</b> page</iref></co>
                See also: <kref type="rel">home-made</kref>
            </def>
        </ar>
        <ar>
            <k>indices</k>
            <def>
                Plural form of word <kref>index</kref>
            </def>
        </ar>
        <ar>
            <k>disc</k>
            <k>disk</k>
            <def>
                <gr><abbr>n.</abbr></gr>
                A flat, circular plate; as, a disk of metal or paper.
            </def>
        </ar>
        <ar>
            <k>CO<sub>2</sub></k>
            <def>
                Carbon dioxide (CO<sub>2</sub>) - a heavy odorless gas formed during respiration.
            </def>
        </ar>
    </lexicon>
</xdxf>
```
