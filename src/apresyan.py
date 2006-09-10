#!/usr/bin/env python
# -*- coding: utf-8 -*-

import re, xml.sax.saxutils
import mdparser

def decode_from_koi(str):
		return str.decode("koi8-r").encode('utf-8')

parser = mdparser.MakeDictParser()

parser.parser_info['version']='0.1'
parser.parser_info['dict_name']='Apresyan.koi'
parser.parser_info['format']='apresyan'
parser.start()

description=decode_from_koi(parser.f.readline())+\
		decode_from_koi(parser.f.readline()).rstrip()

parser.set_dict_info("basename", "Apresyan")
parser.begin()
parser.set_dict_info('full_name',
										 'Новый Большой Англо-Русский Словарь под редакцией Ю.Д. Апресяна')
parser.set_dict_info('lang_from', 'ENG')
parser.set_dict_info('lang_to', 'RUS')
parser.set_dict_info('description', xml.sax.saxutils.escape(description))
parser.info()
parser.abbrs_begin()
abbr_mode = 1
abbr_list = {}
art_beg='<abr_def>'
art_end='</abr_def>'
data_beg='<v>'
data_end='</v>'

ind_pat = re.compile("(?P<ind_val>\w+)&gt;", re.UNICODE)
ex_pat = re.compile("_Ex:\s+(?P<ex_val>[a-zA-Z0-9!\?\.']+[a-zA-Z0-9!\?\.'\s]+)(?P<ex_rest>\s+)", re.UNICODE)
word_pat = re.compile("([\w\.-]+)", re.UNICODE)

for line in parser.f.readlines():
		res = decode_from_koi(line)
		key, data = res.split("  ")

		data = xml.sax.saxutils.escape(data)
		data = ex_pat.sub("\n<ex>\g<ex_val></ex>\g<ex_rest>", data)
		data = ind_pat.sub("\n<b>\g<ind_val>)</b>", data)

		if abbr_mode == 1:
				abbr_list[key] = '<abr>' + key + '</abr>'
		else:
				new_data = ''
				word_list = word_pat.split(data)
				for w in word_list:
						if w in abbr_list:
								new_data = new_data + abbr_list[w]
						else:
								new_data = new_data + w
				data = new_data


		print art_beg + '<k>' + xml.sax.saxutils.escape(key) + '</k>\n' +\
				data_beg + data.rstrip() + data_end + art_end

		if key == '_яп.':
				parser.abbrs_end()
				abbr_mode = 0
				art_beg = "<ar>"
				art_end = "</ar>"
				data_beg = ""
				data_end = ""

parser.end()
