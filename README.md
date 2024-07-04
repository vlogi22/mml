# MML compiler

The steps to develop a full MML compiler imply the adaptation of:
* the scanner (`mml_scanner.l`)
* the parser (`mml_parser.y`)
* the symbol (`targets/symbol.h`)
* the type checker (`targets/type_checker.cpp`)
* the XML writer (for the middle delivery: `targets/xml_writer.cpp`)
* the Postfix writer (for the final delivery: `targets/postfix_writer.cpp`)

[Reference manual](https://web.tecnico.ulisboa.pt/~david.matos/w/pt/index.php/Compiladores/Projecto_de_Compiladores/Projecto_2022-2023/Manual_de_Refer%C3%AAncia_da_Linguagem_MML)
[Libraries](https://web.tecnico.ulisboa.pt/~david.matos/w/pt/index.php/Compiladores/Projecto_de_Compiladores/Material_de_Apoio_ao_Desenvolvimento)
