build:
	find ./ -name "*_withNum.md" -exec rm {} \;
	find ./ -name "*.md" -exec python3 ./AutoNum.py {} \;
	sed -i 's/src="..\/..\//src="\/computer_theory\//g' ./Internet/chapter/*_withNum.md


