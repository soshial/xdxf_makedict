### Changelog (rev. 34), 19 January 2022 
* since rev. 34 the format is fully semantic and cannot store any presentational or visual data
* the language code has no limitation: all languages that exist in BCP47 standard are supported (use https://schneegans.de/lv/?tags=hy-Latn-IT-arevela for validation)
* multilingual dictionaries are now supported: a dictionary may have multiple languages, that are translated from and into. It is also allowed to mark `<k>` and `<def>` tags with `xml:lang`
* description supports line breaks
* transcription info can be directly inside `def` tag

### Changelog (rev. 33), 3 December 2015
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
