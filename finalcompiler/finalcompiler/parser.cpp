#include "parser.h"




/**
  * function   : is_term(string s)
  * description: 这个方法用于判断字符串是否是终结符号
  * param      : s
  * return	   : bool
  */

bool Base::is_term(string s) {
	if (s[0] == '<' && s[s.length() - 1] == '>')
		return false;
	else if (s == "@")
		return false;
	return true;
}


bool is_nonterm(string s) {
	if (s[0] == '<' && s[s.length() - 1] == '>')
		return true;
	return false;
}



/**
  * function   : scan_grammer(string filePath)
  * description: 这个方法用于从文件读入文法，初始化grammer[100], term, non_term
  * param      : filePath
  */

void Base::scan_grammer(string filePath) {
	ifstream t(filePath);
	string line = "";
	int index = 0;//grammer[100]下标
	while (getline(t, line)) //逐行读取文法
	{
		line += " ";
		bool isleft = true;//是否是文法左部
		for (int i = 0, j = 0; i < line.length();) {//通过空格分割文法字符串
			
			// ->
			if (line[i] == '-' && line[i + 1] == '>') {
				i += 3;
				j += 3;
				isleft = false;
			}

			// |
			else if (line[i] == '|') {
				index++;
				grammer[index].left = grammer[index - 1].left;
				i += 2;
				j += 2;
			}

			//文法左部
			else if (line[i] == ' ' && isleft) {
				string temp = line.substr(j, i-j);
				grammer[index].left = temp;
				j = i + 1;
				i++;
			}

			//文法右部
			else if (line[i] == ' ' && !isleft) {
				string temp = line.substr(j, i-j);
				grammer[index].right.push_back( temp);
				j = i + 1;
				i++;
			}

			else i++;
				
		}
		index++;
	}

	grammer_num = index;

	//初始化终结符号和非终结符号表
	for (int i = 0; i < index; i++) {
		non_term.insert(grammer[i].left);
		for (vector<string>::iterator it = grammer[i].right.begin(); it!= grammer[i].right.end(); it++) {
			if (is_term(*it))
				term.insert(*it);//插入终结符号
			else
				non_term.insert(*it);//插入非终结符号
		}
	}

	//输出文法
	/*for (int i = 0; i < index; i++) {
		cout << grammer[i].left << "->";
		for (vector<string>::iterator it = grammer[i].right.begin(); it != grammer[i].right.end(); it++) {
			cout << *it;
		}
		cout << endl;
	}*/
		
}





/**
  * function   : set_first(string target)
  * description: 这个方法用于求指定非终结符号的first集
  * param	   : target
  */

void Base::set_first(string target) {
	
	//匹配文法左部
	for (int i = 0; i < grammer_num; i++) {
		if (grammer[i].left == target) {	
			int num = grammer[i].right.size();
			for (vector<string>::iterator it = grammer[i].right.begin(); it != grammer[i].right.end(); it++) {
				
				//终结符号
				if (is_term(*it)) {
					first_set[target].insert(*it);
					break;
				}

				//空串
				else if (*it == "@") {
					if ((it + 1) == grammer[i].right.end())//最后一个字符串
						first_set[target].insert("@");
				}

				//非终结符号
				else {
					set_first(*it);//递归求first集
					if (first_set[*it].count("@") == 0)//如果这个非终结符号的first集没有@
					{
						set_union(first_set[target].begin(), first_set[target].end(), first_set[*it].begin(), first_set[*it].end(),inserter(first_set[target], first_set[target].begin())); // 求并集
						break;
					}
					else {
						set_union(first_set[target].begin(), first_set[target].end(), first_set[*it].begin(), first_set[*it].end(), inserter(first_set[target], first_set[target].begin())); // 求并集
						first_set[target].erase("@");
					}
				}
			}
		}
	}
}

/**
  * function   : display_first()
  * description: 这个方法用于打印first集和follow集
  */
void Base::display_firstAndFollow() {
	ofstream fout;
	fout.open("FirstAndFollow.txt");
	fout << "文法first集如下：\n";
	for (map<string, set<string>>::iterator it = first_set.begin(); it != first_set.end(); it++) {
		fout<< std::left<<setw(25) << (*it).first << setw(20) << " first集：";
		for (set<string>::iterator it2 = first_set[(*it).first].begin(); it2 != first_set[(*it).first].end(); it2++) {
			fout << *it2 << " ";
		}
		fout << endl;
	}
	fout << "+------------------------------------------------------------------------------------------------------------------------------------------+\n\n";
	fout << "文法follow集如下：\n";
	for (map<string, set<string>>::iterator it = follow_set.begin(); it != follow_set.end(); it++) {
		fout << std::left << setw(25) << (*it).first <<setw(20) <<" follow集：";
		for (set<string>::iterator it2 = follow_set[(*it).first].begin(); it2 != follow_set[(*it).first].end(); it2++) {
			fout << *it2 << " ";
		}
		fout << endl;
	}
}


/**
  * function   : set_follow(string target)
  * description: 这个方法用于求指定非终结符号的follow集
  * param	   : target
  */

void Base::set_follow(string target) {
	for (int i = 0; i < grammer_num; i++) {
		bool flag = false;
		vector<string>::iterator it;
		for (it = grammer[i].right.begin(); it != grammer[i].right.end(); it++)
		{
			if (*it == target) {
				flag = true;
				break;
			}
		}
		if (flag) {
			//右边为空串
			if (it == grammer[i].right.end()) {
				set_follow(grammer[i].left);
				follow_set[*it] = follow_set[grammer[i].left];
			}
			//右边不为空
			else {
				vector<string>::iterator it2;
				for (it2 = it + 1; it2 != grammer[i].right.end(); it2++) {
					if (is_term(*it2)) {
						follow_set[*it].insert(*it2);
						break;
					}
					else if (first_set[*it2].count("@") == 0)
					{
						set_union(follow_set[*it].begin(), follow_set[*it].end(), first_set[*it2].begin(), first_set[*it2].end(), inserter(follow_set[*it], follow_set[*it].begin())); // 求并集
						break;
					}
					else {						
						set_union(follow_set[*it].begin(), follow_set[*it].end(), first_set[*it2].begin(), first_set[*it2].end(), inserter(follow_set[*it], follow_set[*it].begin())); // 求并集
						follow_set[*it].emplace("@");
					}
				}
			}

		}
		if (grammer[i].left == "<program>")
			follow_set["<program>"].insert("$");
	}
}






/**
  * function   : getTopToBottomTable
  * description: 这个方法用于自上而下构造分析表
  * return	   : map<pair<string, string>, int>>
  */
map<pair<string, string>, int> Base::getTopToBottomTable() {
	for (int i = 0; i < grammer_num; i++) 
	{
		for (vector<string>::iterator it0 = grammer[i].right.begin(); it0 != grammer[i].right.end(); it0++) {
			string s = *it0;
			if (is_term(s)) {
				pair<string, string> position;
				position.first = grammer[i].left;
				position.second = s;
				analysisTable_1[position] = i;
				break;
			}
			else if (s == "@" && (it0+1)== grammer[i].right.end()) {
				for (set<string>::iterator it2 = follow_set[grammer[i].left].begin(); it2 != follow_set[grammer[i].left].end(); it2++) {
					pair<string, string> position;
					position.first = grammer[i].left;
					position.second = *it2;
					analysisTable_1[position] = -1;
				}
				break;
			}
			else {
				set<string>::iterator it;
				bool flag = false;
				for (it = first_set[s].begin(); it != first_set[s].end(); it++) {
					if (*it != "@") {
						pair<string, string> position;
						position.first = grammer[i].left;
						position.second = *it;
						analysisTable_1[position] = i;
					}
					else {
						flag = true;
						continue;
					}
				}
				if(!flag)
					break;
			}

		}
	}
	return analysisTable_1;
}









/**
  * function   : display_table
  * description: 这个方法用于打印分析表
  */
//void Base::display_table() {
//	set<string>::iterator it1, it2;
//
//	for (it1 = term.begin(); it1 != term.end(); it1++) {
//		cout << setw(20) << *it1;
//	}
//	cout << endl;
//
//	for (it2 = non_term.begin(); it2 != non_term.end(); it2++) {
//		for (it1 = term.begin(); it1 != term.end(); it1++)
//		{
//			pair<string, string> temp;
//			temp.first = *it2;
//			temp.second = *it1;
//			if (analysisTable.count(temp) == 0) {
//				cout << setw(20) << "";
//			}
//			else {
//				cout << setw(20);
//				int i = analysisTable[temp];
//				if (i == -1) {
//					cout << setw(20) << "@";
//				}
//				else {
//					for (vector<string>::iterator it = grammer[i].right.begin(); it != grammer[i].right.end(); it++) {
//						cout << *it;
//					}
//				}
//			}
//			cout << endl;
//		}
//	}
//}


/**
  * function   : analysis_TopToBottom_Exp()
  * description: 这个方法利用栈分析符号串是否符合语法规范，正确返回0，错误返回行号
  * return	   : int
  * param	   : s
  */
int Base::analysis_TopToBottom_Exp(vector<Token> s) {//分析符号串(自上而下)
	stack<string> left;
	int index = 0;

	s.push_back(Token(0, "$", s[s.size() - 1].col));
	left.push("$");
	left.push("<program>");

	while (index < s.size()) 
	{
		string exp = s[index].value; 
		string leftTop = left.top();

		pair<string, string>temp;
		temp.first = leftTop;
		temp.second = exp;
		if (leftTop==exp) {//相等则都出栈
			left.pop();
			index++;
		}	
		else if (analysisTable_1.count(temp) != 0) {
			left.pop();
			int i = analysisTable_1[temp];
			if (i == -1) {
				continue;//空串
			}
			else {
				vector<string> exchange = grammer[i].right;
				vector<string>::iterator it;
				for (it = exchange.end()-1; it != exchange.begin(); it--) {
					left.push(*it);
				}
				left.push(*it);
			}
		}
		else {
			return s[index].col;
		}
	}

	if (left.empty())
		return 0;
	
}



/**
  * function   : analysis_BottomToTop_Exp()
  * description: 这个方法利用栈分析符号串是否符合语法规范，正确返回0，错误返回行号
  * return	   : int
  * param	   : s
  */
//int Base::analysis_BottomToTop_Exp(vector<Token> s) {
//
//}




/**
  * function   : parser()
  * description: 语法分析
  */
void Base::parser(vector<Token> tokenList) {
	scan_grammer("Testgrammer.txt");//扫描文法
	//for (int i = 0; i < grammer_num; i++) {//求first集
	//	set_first(grammer[i].left);		
	//}
	//for (int i = 0; i < grammer_num; i++) {//求follow集
	//	set_follow(grammer[i].left);
	//}
	//
	//display_firstAndFollow();//打印first集和follow集

	//map<pair<string, string>, int> table = getTopToBottomTable();//构建自上而下符号表
	//
	//int flag = analysis_BottomToTop_Exp(tokenList);//分析
	//if (flag==0) {
	//	cout <<  "符合语法规范" << endl;
	//}
	//else
	//	cout<<"第"<<flag<<"行不符合语法规范" << endl;
}

bool judgeIs_SameVecOfNode(vector<node> tmp, vector<node> v);
bool judgeIs_SameVecOfString(vector<string> tmps, vector<string> vs);

//由文法生成项目集簇
void Base:: generateProjectSet() {
	//当前项目集
	I tmp;
	int id = 0;
	tmp.id = id;
	id++;
	tmp.vec.push_back(grammer[0]);
	queue<I> que;//等待完成的项目集队列
	que.push(tmp);

	vector<vector<node> > vecv;//
	vecv.push_back(tmp.vec);
	map<int, vector<node> > mapvi;
	mapvi[tmp.id] = tmp.vec;

	while (!que.empty()) {
		tmp = que.front();
		que.pop();

		int size = tmp.vec.size();
		bool first = true;
		set<string> already;
		while (first || size != tmp.vec.size()) {
			first = false;
			size = tmp.vec.size();
			set<string> left;
			for (int i = 0; i < size; i++) {
				node t = tmp.vec[i];//当前文法；
				if (t.index < t.right.size()) {//如果点后有文法符号
					string s = t.right[t.index];
					if (!is_term(s) && already.count(s)==0 ) {//如果是非终结符号且没被添加
						left.insert(t.right[t.index]);//把点后的非终结文法符号加到left里
					}
				}
			}
			for (set<string>::iterator it = left.begin(); it != left.end(); it++) {
				for (int j = 0; j < grammer_num; j++) {
					if (grammer[j].left == *it) {
						tmp.vec.push_back(grammer[j]);
					}
				}
				already.insert(*it);
			}
		}
		//以上构建完了项目集的文法集，下面构建转化关系

		set<string> stringSet;
		stringSet.insert("@");
		for (int i = 0; i < tmp.vec.size();i++) {
			node t = tmp.vec[i];//当前文法；
			if (t.index < t.right.size()) {//如果点后有文法符号
				string s = t.right[t.index]; //文法符号
				if (stringSet.count(s) == 0) {
					stringSet.insert(s);
				}
				else {
					continue;
				}
				vector<node> mainGrammerOfProjectSet; //项目集的主要文法 ，其他文法可由主文法推出
				for (int j = 0; j < tmp.vec.size(); j++) { //遍历tmp项目集文法，寻找点后为文法符号s的文法
					node newT = tmp.vec[j];
					if (newT.index < newT.right.size() && newT.right[newT.index] == s) {
						newT.index++;
						mainGrammerOfProjectSet.push_back(newT);
					}
				}
				//判断是否已生成该项目集，否则生成新项目集加到projectSet里面
				bool exist = false;
				for (int j = 0; j < vecv.size(); j++) {
					if (judgeIs_SameVecOfNode(vecv[j], mainGrammerOfProjectSet)) {
						exist = true;
						break;
					}
				}
				if (exist) {
					int id = -1;
					for (map<int, vector<node> >::iterator it = mapvi.begin(); it != mapvi.end(); it++) {
						if (judgeIs_SameVecOfNode(it->second, mainGrammerOfProjectSet)) {
							id = it->first;
							break;
						}
					}
					if (id == -1) cout << "mapvi错误" << endl;
					tmp.m[s] = id;
				}
				else {
					I newI;
					newI.id = id;
					tmp.m[s] = newI.id;
					id++;
					newI.vec = mainGrammerOfProjectSet;
					que.push(newI);
					vecv.push_back(mainGrammerOfProjectSet);
					mapvi[newI.id] = mainGrammerOfProjectSet;
				}
			}
		}
		printI(tmp);
		projectSet.push_back(tmp);
	}
}

void Base::printI(I pi) {
	cout << "I" << pi.id << endl;
	vector<node> v = pi.vec;
	for (int i = 0; i < v.size(); i++) {
		cout << v[i].left << "->";
		for (int j = 0; j < v[i].right.size(); j++) {
			if (j == v[i].index) cout << ".";
			cout << v[i].right[j] << " ";
		}
		if (v[i].index == v[i].right.size()) cout << ".";
		cout << endl;
	}
	for (map<string, int>::iterator it = pi.m.begin(); it != pi.m.end(); it++) {
		cout << it->first << " " << it->second << endl;
	}
	cout << endl;
}



//bool Base::is_existTheI(vector<node> v,int & tmpid,I tmp) {
//	vector<node> vec(tmp.vec.begin(),tmp.vec.begin() + tmp.num); //tmp.num和v size一样后 判断tmp和v是不是完全一样
//	if (judgeIs_SameVecOfNode(vec, v)) {
//		tmpid = tmp.id;
//		return true;
//	}
//
//	for (int i = 0; i < projectSet.size(); i++) {
//		vector<node> tmp(projectSet[i].vec.begin(), projectSet[i].vec.begin() + projectSet[i].num); //tmp.num和v size一样后 判断tmp和v是不是完全一样
//		if (judgeIs_SameVecOfNode(tmp, v)) {
//			tmpid = projectSet[i].id;
//			return true;
//		}
//	}
//	return false;
//}
//便利函数
bool judgeIs_SameVecOfNode(vector<node> tmp, vector<node> v) {
	if (tmp.size() != v.size()) return false;
	for (int j = 0; j < v.size(); j++) {//遍历一遍v，看看tmp里有没有v[j]
		bool has_vj = false;
		for (int k = 0; k < tmp.size(); k++) {
			if (v[j].index == tmp[k].index && v[j].left == tmp[k].left) {
				vector<string> tmps = tmp[k].right;
				vector<string> vs = v[j].right;
				if (judgeIs_SameVecOfString(tmps, vs)) {
					has_vj = true;
					break;
				}
			}
		}
		//如果没找到，就不一样
		if (has_vj == false) return false;
	}
	return true;
}
//便利函数
bool judgeIs_SameVecOfString(vector<string> tmps, vector<string> vs) {
	if (tmps.size() == vs.size()) {
		for (int z = 0; z < tmps.size(); z++) {
			if (tmps[z] != vs[z]) {
				return false;
			}
		}
		return true;
	}
	return false;
}

void Base::generateSL0Table() {
	for (int i = 0; i < projectSet.size(); i++) {
		I currentI = projectSet[i];
		for (int j = 0; j < currentI.vec.size(); j++) {
			node currentNode = currentI.vec[j];
			if (currentNode.index == 1 && currentNode.left == grammer[0].left &&
				currentNode.right.size() == 1 && currentNode.right[0] == grammer[0].right[0]) {
				ACTION[make_pair(currentI.id, "$")] = make_pair("acc", -1);
			}
			if (currentNode.index < currentNode.right.size()) {
				string a = currentNode.right[currentNode.index];
				if (is_term(a) && currentI.m.count(a) != 0) {
					ACTION[make_pair(currentI.id, a)] = make_pair("s", currentI.m[a]);
				}
			}
			else if (currentNode.index == currentNode.right.size()) {
				set_follow(currentNode.left);
				set<string> s = follow_set[currentNode.left];
				for (set<string>::iterator it = s.begin(); it != s.end(); it++) {
					int id;
					//寻找规约产生式的id；
					for (id = 0; id < grammer_num; id++) {
						if (currentNode.left == grammer[id].left &&
							judgeIs_SameVecOfString(currentNode.right, grammer[id].right)) break;
					}
					if (id == grammer_num) cout << "没有找到归约产生式" << endl;
					ACTION[make_pair(currentI.id, *it)] = make_pair("r", id);
				}
			}
		}
		for (map<string, int>::iterator it = currentI.m.begin(); it != currentI.m.end(); it++) {
			if (!is_term(it->first)) {
				GOTO[make_pair(currentI.id, it->first)] = it->second;
			}
		}
	}
}

void Base::printSL0Table() {
	vector<vector<string>> SL0Table;
	int id = 0;
	vector<string> symbol;
	//symbol.push_back("状态");
	map<string, int> symbolToId;
	for (set<string>::iterator it = term.begin(); it != term.end(); it++) {
		symbol.push_back(*it);
		symbolToId[*it] = id;
		id += 1;
	}
	symbol.push_back("$");
	symbolToId["$"] = id;
	id += 1;
	for (set<string>::iterator it = non_term.begin(); it != non_term.end(); it++) {
		symbol.push_back(*it);
		symbolToId[*it] = id;
		id += 1;
	}
	//SL0Table.push_back(symbol);
	for (int i = 0; i < projectSet.size(); i++) {
		vector<string> vec;
		for (int j = 0;j<term.size()+1;j++) {
			if (ACTION.count(make_pair(i, symbol[j])) != 0) {
				pair<string,int> p = ACTION[make_pair(i, symbol[j])];
				if (p.first == "acc") vec.push_back(p.first);
				else vec.push_back(p.first+to_string(p.second));
			}
			else vec.push_back(" ");
		}

		for (set<string>::iterator it = non_term.begin(); it != non_term.end(); it++) {
			if (GOTO.count(make_pair(i, *it)) != 0) {
				vec.push_back(to_string(GOTO[make_pair(i, *it)]));
			}
			else vec.push_back(" ");
		}
		SL0Table.push_back(vec);
	}

	cout << setw(6) << setfill(' ') << "状态";
	for (int j = 0; j < symbol.size(); j++) {
		cout << setw(6) << setfill(' ') << symbol[j];
	}
	cout << endl;
	for (int i = 0; i < SL0Table.size(); i++) {
		cout << setw(6) << setfill(' ') << i;
		for (int j = 0; j < SL0Table[j].size(); j++) {
			cout << setw(6) << setfill(' ') <<SL0Table[i][j];
		}
		cout << endl;
	}
}

void Base::generate_FirstAndFollow() {
	for (set<string>::iterator it = non_term.begin(); it != non_term.end(); it++) {
		set_first(*it);
	}
	for (set<string>::iterator it = non_term.begin(); it != non_term.end(); it++) {
		set_follow(*it);
	}
	cout << "first集" << endl;
	for (map<string, set<string>>::iterator it = first_set.begin(); it != first_set.end(); it++) {
		cout << it->first << ":" << endl;
		for (set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			cout << *it2 << " ";
		}
		cout << endl;
	}
	cout << "follow集" << endl;
	for (map<string, set<string>>::iterator it = follow_set.begin(); it != follow_set.end(); it++) {
		cout << it->first << ":" << endl;
		for (set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			cout << *it2 << " ";
		}
		cout << endl;
	}
}
