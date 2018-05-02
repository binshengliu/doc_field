# doc_field
Dump field text from Indri index using a document number (not ID)

## Usage

```
$ ./doc_field
Built with Indri release 5.11
Usage:   ./doc_field <repo> <field> <docno>
Example: ./doc_field cw09b-index/ inlink clueweb09-en0000-00-00000
```

## Example

```
$ ./doc_field ../cw09b-index/CW09B-NoStop-NoFields-Index/ inlink clueweb09-en0000-00-00000 2> /dev/null
00000 nrt realestate 00000 nrt realestate
```
