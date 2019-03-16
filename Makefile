######################
#ブロック崩しMakefile#
######################

#ブロック崩し本体
TGT	= block
#データエディタ
DEDT	= dataeditor
#コンパイラ
CXX	= g++
#デバッグオプション
DBG	= -g
#最適化オプション
OPT	= -O 
#ランキングCGI
CGI	= blockranking
#データクリーナー
DCLN	= datacleaner
#C++11を利用するオプション
C++11	= -std=c++0x 
#(n)cursesのオプション
CUS	= -lncursesw

##############################################

#ブロック崩しコンパイル
$(TGT): $(TGT).cpp *.hpp
	$(CXX) $(OPT)$(TGT).cpp -o $(TGT) $(CUS)

#デバッグ用コンパイル
debug: $(TGT).cpp *.hpp
	$(CXX) $(DBG)$(TGT).cpp -o $(TGT) $(CUS)

clean:$(TGT)
	rm $(TGT)

#データエディタコンパイル
dedit: $(DEDT).cpp scoredata.hpp
	$(CXX) $(C++11)$(DEDT).cpp -o $(DEDT)

#デバッグ用コンパイル
deditdebug: $(DEDT).cpp scoredata.hpp
	$(CXX) $(DBG)$(C++11)$(DEDT).cpp -o $(DEDT)

#ランキングCGIコンパイル
cgi: $(CGI).cpp httplib.hpp scoredata.hpp
	$(CXX) $(C++11)$(CGI).cpp -o $(CGI).cgi

cgidebug: $(CGI).cpp httplib.hpp scoredata.hpp
	$(CXX) $(DBG)$(C++11)$(CGI).cpp -o $(CGI).cgi

#データクリーナー用コンパイル
dcleaner: $(DCLN).cpp scoredata.hpp
	$(CXX) $(DCLN).cpp -o $(DCLN)

#デバッグ用コンパイル
dcleanerdebug: $(DCLN).cpp scoredata.hpp
	$(CXX) $(DBG)$(DCLN).cpp -o $(DCLN)
