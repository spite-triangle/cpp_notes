build:
	find ./theory/chapter/ -name "*_withNum.md" -exec rm {} \;
	find ./theory/chapter/ -name "*.md" -exec python3 ./AutoNum.py {} \;
	sed -i 's/src="..\/..\//src="\/cpp_notes\//g' ./theory/chapter/*_withNum.md
	find ./CppConcurrency/chapter/ -name "*_withNum.md" -exec rm {} \;
	find ./CppConcurrency/chapter/ -name "*.md" -exec python3 ./AutoNum.py {} \;
	sed -i 's/src="..\/..\//src="\/cpp_notes\//g' ./CppConcurrency/chapter/*_withNum.md
	find ./http/chapter/ -name "*_withNum.md" -exec rm {} \;
	find ./http/chapter/ -name "*.md" -exec python3 ./AutoNum.py {} \;
	sed -i 's/src="..\/..\//src="\/cpp_notes\//g' ./http/chapter/*_withNum.md
	find ./testTools/chapter/ -name "*_withNum.md" -exec rm {} \;
	find ./testTools/chapter/ -name "*.md" -exec python3 ./AutoNum.py {} \;
	sed -i 's/src="..\/..\//src="\/cpp_notes\//g' ./testTools/chapter/*_withNum.md
	find ./workflow/chapter/ -name "*_withNum.md" -exec rm {} \;
	find ./workflow/chapter/ -name "*.md" -exec python3 ./AutoNum.py {} \;
	sed -i 's/src="..\/..\//src="\/cpp_notes\//g' ./workflow/chapter/*_withNum.md
			

