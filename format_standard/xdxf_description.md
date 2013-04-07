                                    XDXF standard;  Draft 032beta;  March 2013

# Description
XDXF stands for XML Dictionary Exchange Format, and as its name implies, specifies a format for dictionary data files.
Some dictionary software supports XDXF natively (see for ex., GoldenDict, XDClient).

Each dictionary is located in its own folder, the name of the folder is used as ID, and must contain only Latin characters and must not contain spaces or other special characters. So, if the dictionary name is "Webster's Unabridged Dictionary published in 1913" then the folder name should be something like "Webster1913". The dictionary file itself is always "dict.xdxf". It is recommended for each dictionary to have a set of icons for toolbars and a large icon for the front page. The sizes should be: 16x16, 32x32, 512x512.  And the filenames would be icon16.png icon32.png and icon512.png respectively.
Note that all file names are case sensitive.

All XDXF dictionary text files (those with .xdxf extension) are in XML format with any Unicode encoding (usually UTF-8).
Any other encodings are strictly prohibited.

## Changelog:

* `<meta_info>`, `<lexicon>` were introduced to have simpler structure of the dict file
* semantic relations' section revamped to reduce the number of tags
* definition usage frequency attribute added
* phrasemes (aka phraseologisms) moved into `<ex>` section; also added proverbs to `<ex>`
* file version and date, dictionary edition and date, dict source url added to `<meta_info>`

# XDXF Tags:
## xdxf

`<xdxf lang_from="XXX" lang_to="XXX" format="FORMAT" revision="DD">` is the **root element**. It must have 4 attributes:
* `lang_from` and `lang_to` values are 3-letter language codes from [ISO 639-3 standard](http://sil.org/iso639-3/)
    and represents the language of key-phrases and definitions respectively.
* The `format` attribute specifies the default formatting for the dictionary and might be either `visual` or
    `logical`. The default format might be overwritten for specific articles as described below.
    * In visual format, the articles are formatted visually and are intended to be shown by
        dictionary programs (shells) as is without inserting or removing any spaces or EOLs.
        However, shells may mark the content of logical tags with different colors.
    * In logical format, the articles are not formatted visually and shells are responsible
        for formating them before presenting them to the user.
* `revision` attribute specifies format version that your XDXF file is formatted in.  
  *Tip*: this attribute is obligatory, but it was first introduced in recent format standard, so it might be
    absent in some old xdxf files.

The structure of a file is divided into 2 parts: the ``<meta_info>`` and the ``<lexicon>``:
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
## meta_info

`<meta_info>` is the container for all meta information about the dictionary. It contains:  
  1. `<title>` The short title of the dictionary written in English  
  2. `<full_title>` Full name of the dictionary, like it would appear on the book cover.  
     *Tip*: Can contain a non-English title.  
  3. `<publisher>` The official publisher of the dictionary; optional.  
  4. `<authors>` contains a list of `<author>` tags and represents all people (organizations) that took part in making dictionary: lexicographers, proofreaders, programmers etc. It is optional.
     * `<author role="xxx">` One tag for each author.
     *Tip*: some authors might have >1 roles: in this case (s)he should be listed using two `<author>` tags  
  5. `<description>` of the dictionary is in free words or an annotation/resume/reader's note from the publisher. It is recommended to include the following: Copyright, License, whence this file can be downloaded, whence the unformatted file (i.e. the original dictionary file before the conversion into XDXF format) can be downloaded, whence the original unformatted dictionary file was obtained, and link to the script which was used to convert the original unformatted dictionary file into XDXF format. The description may contain XHTML tags that are allowed in XDXF and specified below.  
  6. `<abbreviations>` section contains a list of `<abbr_def>` tags. It contains all abbreviations used for labels in the dictionary (grammar labels, for example). `<abbr_def>` tag defines an abbreviation and contains two types of tags:
     * `<abbr_k>` (stands for abbreviation key): abbreviated text.
     * `<abbr_v>` (stands for value): full text.
      
  `<abbr_def>` might contain a `type` attribute, which states which type of label this abbreviation is: 
     * `<abbr_def type="grm">` — stating grammatical features of word (noun, past participle etc.)
     * `<abbr_def type="stl">` — stylistical properties of a word (vulgar, archaic, poetic etc.)
     * `<abbr_def type="knl">` — area/field of knowledge (computers, literature, culinary, typography etc.)
     * `<abbr_def type="aux">` — simple subsidiary words like ('e.g.', 'i.e.', 'cf.', 'also', 'rare' etc.).
     * `<abbr_def type="oth">` — others
      
  *Tip*: Note that each abbreviation definition may contain more than one `<abbr_k>` per `<abbr_def>` to specify synonyms like "Ave." and "Av.", but `<abbr_k>` tag can be only one.  

    ```xml
    <abbreviations>
        <abbr_def><abbr_k>n.</abbr_k><abbr_v>noun</abbr_v></abbr_def>
        <abbr_def><abbr_k>v.</abbr_k><abbr_v>verb</abbr_v></abbr_def>
        <abbr_def><abbr_k>Av.</abbr_k><abbr_k>Ave.</abbr_k><abbr_v>Avenue</abbr_v></abbr_def>
    </abbreviations>
    ```
  7. `<file_ver>`, `<creation_date>` are obligatory info.  
   `<last_edited_date>`, `<dict_edition>`, `<publishing_date>`, `<dict_src_url>` are optional meta info.  
   All dates should be formatted as `DD-MM-YYYY`. When the date is not fully known, there should be zeros: `05-00-2011`.

## lexicon
`<lexicon>` is the container for all `<ar>` (article) tags.
### ar
The `<ar>` article tag groups together all the stuff related to one key-phrase. It can have an optional attribute `f`, eg. `<ar f="x">` which may have value either `v` (visual) or `l` (logic) and can be used to overwrite the default dictionary format, which was specified in the `<xdxf>` tag.  
NB! Remember, that visual format is NOT recommended! XDXF is developed especially for logically structured dicts and the visual format was introduced only to be compatible with dicts converted from old plain-text formats.  
The following tags are allowed only in between the `<ar></ar>` tags:
  1. `<k>` Key phrase is a phrase by which an article containing it can be found alphabetically.
   Article may contain more than one key phrase and always at least one. If there are more than one `<k>`,
   while the article is opened by one key-phrase, the dictionary software should show all other variants in the
   article. Tag `<k>` may not be nested in another `<k>`.
   * `<opt>` Marks optional part of key-phrase. The article is searched by the `<k>` contents without `<opt>` contents,
   but showed in the article with it. Tag `<opt>` might be used only in between `<k></k>` tags.
  2. `<def>` This tag marks the whole body of word article, each definition itself and a group of definitions which fall into a certain category.  
   *Tip*. Those categories could be parts of speech or different etymology.
   * Note that `<def>` tags can nest and they usually do.
   * The `<def>` tag must be inside `<ar>` even if the article is simple and there is nothing to group.
   * In articles with visual format `<def>` tags do not effect the formatting.  
   * They might have a 'cmt' attribute, that helps disambiguate this exact definition from others
   * They might have a unique lowcase alphanumerical `id` attribute [1-90a-z].
   * They might have an integer/float attribute `freq`: some absolute/relative frequency value of the definition.  
   * For articles that have logical format shells may use `<def>` tag in a similar way as `<blockquote>` tag is used in HTML, or may also put '1)', '2)'... or '1.', '2.'... or 'A.', 'B.'... etc. before each definition, and increase the font size of '1)', '2)'... etc. according to the nesting level.  
     Inside the `<def>` tag the additional following tags may be used:  
       1. `<gr>` Specifies grammar information about the word.
       2. `<tr>` Marks transcription/pronunciation information; IPA symbols are the default.
            Might also have "mode" attribute with values "X-SAMPA" or "erkIPA".
       3. `<kref>` Simple reference to another key-phrase, which is located in the same file.
       4. `<dtrn>` This tag marks Direct Translation of the key-phrase. Should be used to help dictionary software
            automatically know the simplest translations of the key-phrase — this might be useful for:
            * automatic extraction of data for tooltip translations (e.g. qDictionary)
            * make visible `<dtrn>` in the word-list to avoid too frequent "full article" look-ups
            These words might also be automatically `<kref>`, if opposite language pair is present
       5. `<rref>` Reference to a Resource file, which should be located in the same folder as dictionary.
            * "start" and "size". Optional attributes `start` and `size` are necessary for audio and video files,
            when the reference points to a certain part of a large file. The attribute `start`
            specifies position in the file of the first byte of the chunk of interest, and `size`
            specifies its length in bytes. If the "start" attribute is omitted then it is assumed
            that it is 0. If the `size` attribute is omitted then it is assumed that the file
            should be played up to the end. Using: `<rref start="xxx" size="xxx">`
       6. `<iref href="http://www.somewebsite.com">` Reference to an Internet resource.
       7. `<abbr>` Marks an abbreviation that is listed in the `<abbreviations>` section.
       8. `<c c="xxxxxx">...</c>` (c c stands for Color Code) Marks text with a given color.
            The syntax for "c" attribute is the same as for "color" attribute of "font" tag in HTML.
            If the color attribute is omitted, the default color is implied.  The default color is
            chosen by the dictionary program.
       9. `<ex>` Marks the text of an example (usually shown in a different color by the program).
        
            *Idexing*: Usually stays indexed, but users should be able to configure whether they want examples to be
                indexed and searched for.
            Attribute "type" might be:
            * `exm` - susal examples with or without translations
            * `phr` - might contain any type of phrasemes (idioms, collocations, clichés etc.)
            * `prv` - proberbs
            
            `<ex_main>` is for the original phrase of the example; should always be
            
            `<ex_tran>` is optional; may be multiple translations.
       10. `<co>` Marks the text of an editorial comment (shown in a different color by the program).
        
            *Idexing*: might be indexed.
       11. `<sr>` There is a section dedicated to sematic relations to other words like synonyms, holonyms, hypernyms etc. It uses `<kref>` to address other word with additional tag attributes. `<sr>` section looks like:  
           `<sr><kref type="syn" comt="obsolete">game</kref><kref type="hol" comt="partly">play</kref></sr>`  
           *Indexing*: should not be indexed as text.
            
            Possible ``type`` values:
             * `syn` and `ant` — synonyms and antonyms
             * `hpr` and `hpn` — hyperonyms and hyponims (incorporate troponyms)
             * `par` and `spv` — paronyms and spelling variants
             * `mer` and `hol` — meronyms and holonyms
             * `ent` — entailment, special verb category: the v. Y is entailed by X if by doing X you must be doing Y
                    might also be used for nouns: doing, having or being this noun entails some other thing
                    (doing a "felony" entails being a "criminal")
             * `rel` — denotes relevance (for ex., "handsome" and "ugly" are relevant to "appearance")
       12. `<etm>` Etymology information about the word.
       13. `<di>` Marks the part of `<def>`'s text that should not be indexed. Might be used only inside `<def>` and some its children: `<co>`, `<ex>`, `<etm>`, `<phr>`

### Non-XDXF Tags:

For visually formatted articles in addition to XDXF tags, the following XHTML tags are allowed:
  `<sup>`, `<sub>`, `<i>`, `<b>`, `<big>`, `<small>`, `<blockquote>`
Their syntax and semantics are the same as in XHTML.

# Examples:

## Visual format
(NOT RECOMMENDED, only for compatibility with old converted dictionaries)
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
## Logical format
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
                <def><dtrn>дом</dtrn>, at home - дома, у себя; make yourself at home - будьте как дома</def>
                <co>XDXF <iref href="http://xdxf.sourceforge.net"><b>Home</b> page</iref></co>
                See also: <kref>home-made</kref>
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
