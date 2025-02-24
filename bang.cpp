#include <clocale>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <locale>
#include <ncurses.h> 
#include <vector>
#include <string>
#include <fstream>
#include <locale.h>
#include <codecvt>
#include <filesystem>

struct wininfo{
    WINDOW* win;
    std::string mode;
    std::string comd;
    std::string filename;
    std::vector<std::string> Veiwable_buf;
    std::vector<std::string> Total_buf;
    int line_pos;
    int row_pos;
    int col_pos;
    int print_row_pos;
    int max_line = 0; 
};

enum customcolour {
    COLOR_LIGHTBLUE = 90,
    COLOR_LIGHTGREEN = 91,
    COLOR_ORAGNE = 92,
    COLOR_NICEGREY = 93,
};

void update(wininfo &window_wininfo){
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;


    for(uint8_t i = 0; i < getmaxy(stdscr) - 2; i++){
        if(i <= window_wininfo.Veiwable_buf.size() - 1){
            std::wstring veiw_wid = converter.from_bytes(window_wininfo.Veiwable_buf[i]);
            mvwaddwstr(stdscr, window_wininfo.print_row_pos, 0, veiw_wid.c_str());
            window_wininfo.print_row_pos++;
        }else{
            mvwaddwstr(stdscr, window_wininfo.print_row_pos, 0, L"~");
            window_wininfo.print_row_pos++;
        }
    }
    window_wininfo.print_row_pos = 0;
    short col1 = 0;
    short col2 = 0;

   
    if(window_wininfo.mode == " NORMAL "){col1 = COLOR_LIGHTBLUE, col2 = COLOR_BLACK;}
    else if(window_wininfo.mode == " INSERT "){col1 = COLOR_LIGHTGREEN, col2 = COLOR_BLACK;}
    else if(window_wininfo.mode == " COMMAND "){col1 = COLOR_ORAGNE, col2 = COLOR_BLACK;}

    init_color(COLOR_LIGHTBLUE, 565, 690, 972);
    init_color(COLOR_LIGHTGREEN, 643, 894, 643);
    init_color(COLOR_ORAGNE, 1000, 369, 0);
    init_color(COLOR_NICEGREY, 204, 204, 263);



    init_pair(1, col1, col2);
    init_pair(2, col1, col2);
    init_pair(3, COLOR_WHITE, COLOR_NICEGREY);
    init_pair(4, COLOR_NICEGREY, COLOR_BLACK);
    
    std::wstring wide_str = converter.from_bytes(window_wininfo.mode);
    attron(COLOR_PAIR(2));

     
    std::wstring icon = L"\ue0b6";
    mvwaddwstr(stdscr, getmaxy(stdscr) - 2, 0, icon.c_str());
    
    attroff(COLOR_PAIR(2));
    
    attron(A_REVERSE);
    attron(A_BOLD);
    attron(COLOR_PAIR(1));
    
    
    mvwaddwstr(stdscr, getmaxy(stdscr) - 2, 1, wide_str.c_str());
    
    attroff(COLOR_PAIR(1));
    attroff(A_BOLD);
    attroff(A_REVERSE);

    
    attron(COLOR_PAIR(3));
    mvwaddwstr(stdscr, getmaxy(stdscr) - 2, wide_str.length() + 1, converter.from_bytes(" " + window_wininfo.filename).c_str());
    attroff(COLOR_PAIR(3));

    attron(COLOR_PAIR(4));
    icon = L"\ue0b4";
    mvwaddwstr(stdscr, getmaxy(stdscr) - 2, wide_str.length() + window_wininfo.filename.length() + 2, icon.c_str());
    attroff(COLOR_PAIR(4));
    

    std::wstring wide_str1 = converter.from_bytes(window_wininfo.comd);
    mvwaddwstr(stdscr, getmaxy(stdscr) - 1, 0, wide_str1.c_str());
    refresh();
	move(window_wininfo.row_pos, window_wininfo.col_pos);
}
int handle_normal(wininfo &mainwininfo);

int handleinsert(wininfo &mainwininfo){
	mainwininfo.mode = " INSERT ";
    mainwininfo.comd = "--INSERT--";
	update(mainwininfo);
    
    while(true){
		const char ch = wgetch(mainwininfo.win);
		
		if(ch != ERR){
            clear();
            switch((int)ch){
                case 27:{
                    curs_set(2);
                    update(mainwininfo);
                    handle_normal(mainwininfo);
                    break;
                }
                case 4:{
                    //arr <- this way
                    if(mainwininfo.col_pos > 0){
                        mainwininfo.col_pos--;
                        
                        wmove(mainwininfo.win, mainwininfo.row_pos, mainwininfo.col_pos);
                        update(mainwininfo);
                    }
                    update(mainwininfo);
                    break;
                }
                case 5:{
                    //arr -> this way
                    if(mainwininfo.col_pos <= mainwininfo.Total_buf[mainwininfo.row_pos].size() - 1){
                        mainwininfo.col_pos++;
                    
                        wmove(mainwininfo.win, mainwininfo.row_pos, mainwininfo.col_pos);
                    }
                    update(mainwininfo);
                    break;
                }
                case 2:{
                    //up arr 
                    if(mainwininfo.row_pos < mainwininfo.Veiwable_buf.size()){
                        mainwininfo.row_pos++;
                        wmove(mainwininfo.win, mainwininfo.row_pos, mainwininfo.col_pos);
                    }

                    update(mainwininfo);
                    break;
                }
                case 7:{
                    if(mainwininfo.col_pos == 0){
                        if(mainwininfo.line_pos != 0){
                            mainwininfo.line_pos--;
                            mainwininfo.row_pos--;
                            mainwininfo.col_pos = mainwininfo.Total_buf[mainwininfo.line_pos].length();
                                
                            mainwininfo.Veiwable_buf.erase(mainwininfo.Veiwable_buf.begin() + mainwininfo.row_pos + 1);
                            mainwininfo.Total_buf.erase(mainwininfo.Total_buf.begin() + mainwininfo.line_pos + 1);
                            
                            update(mainwininfo);
                        }else{
                            update(mainwininfo);
                        }
                    }else{
                        mainwininfo.Total_buf[mainwininfo.line_pos].erase(mainwininfo.col_pos - 1, 1);
                        mainwininfo.Veiwable_buf[mainwininfo.row_pos].erase(mainwininfo.col_pos - 1, 1);
                        mainwininfo.col_pos--;

                           update(mainwininfo);
                    }
                    break;
                }  
                case 10:{
                    if(mainwininfo.row_pos  + 1 < getmaxy(stdscr) - 2){
                            mainwininfo.col_pos = 0;
                            mainwininfo.row_pos++;
                            mainwininfo.line_pos++;
                        
                            mainwininfo.Veiwable_buf.push_back("");
                            mainwininfo.Total_buf.push_back("");
                            update(mainwininfo);
                    }else{
                        update(mainwininfo);
                    }
                    break;
                }
                default:{
        		    mainwininfo.Veiwable_buf[mainwininfo.row_pos].insert(mainwininfo.col_pos, 1, ch);
		            mainwininfo.Total_buf[mainwininfo.line_pos].insert(mainwininfo.col_pos, 1, ch);
		            mainwininfo.col_pos++;
		            update(mainwininfo);
                    break;
                }
            }
		}
            
    }
    return 0;
};

int handelcmd(wininfo &mainwininfo){
    std::string PreCallComd = mainwininfo.comd;
    std::string PreCallmode = mainwininfo.mode;
    
    mainwininfo.mode = " COMMAND ";
    mainwininfo.comd = "";
    clear();
    update(mainwininfo);
    
   
    char ch;
    while((ch = wgetch(mainwininfo.win)) != 10){
        if(ch != ERR){
            clear();
            if(ch == 7){
                if(mainwininfo.comd.length() == 0){
                    break;                
                }else{
                    mainwininfo.comd = mainwininfo.comd.substr(0, mainwininfo.comd.length() - 1);
                    update(mainwininfo);
                }
            }else{
                mainwininfo.comd+=ch;
                update(mainwininfo);
            }
        }else if(ch == 10){
            break;
        }
    }

    if(mainwininfo.comd == "q"){
        if(mainwininfo.win == stdscr){
            endwin();
            exit(0);
        }else{
            delwin(mainwininfo.win);
        }
    }else if(mainwininfo.comd == "save"){
        std::ofstream file(mainwininfo.filename);
        for(auto &i : mainwininfo.Total_buf){
            file << i << "\n"; 
        }
        file.close();
    }else if(mainwininfo.comd.substr(0, 8) == "setname:"){
        int pos = 9;
        while(mainwininfo.comd[pos] == ' '){
            pos++;
        }
        mainwininfo.filename = mainwininfo.comd.substr(pos);
        clear();
        update(mainwininfo);
    }
    
    mainwininfo.mode = PreCallmode;
    mainwininfo.comd = PreCallComd;
    clear();
    update(mainwininfo);
    return 0;
}
int handle_normal(wininfo &mainwininfo){
	mainwininfo.mode = " NORMAL ";
    mainwininfo.comd = "--NORMAL--";
	update(mainwininfo);
    
    while(true){
		const char ch = wgetch(mainwininfo.win);
		
		if(ch != ERR){
            clear();
            switch((int)ch){
                case 105:{
                    update(mainwininfo);
                    handleinsert(mainwininfo);
                    break;
                }  
                case 58:{ 
                    update(mainwininfo);
                    handelcmd(mainwininfo);
                }
                default:{
		            update(mainwininfo);
                    break;
                }
            }
		}else{
            update(mainwininfo);
        }
            
    }
    return 0;};

int main(int argc, char** argv){
    std::string filename("Uintiled.file");
    
    initscr();

    wininfo mainwininfo = {
        .win = stdscr,
        .mode = " NORMAL ",
        .comd = "--NORMAL--",
        .filename = filename,
        .Veiwable_buf = {""},
        .Total_buf = {""},
        .line_pos = 0,
        .row_pos = 0,
        .col_pos = 0,
        .print_row_pos = 0
    };


    if(argc > 1){
        mainwininfo.filename = argv[1];
        std::fstream File(argv[1]);
        if(File.is_open()){
            std::string Line;
            while(std::getline(File, Line)){
                mainwininfo.Total_buf.push_back(Line);
            }
        }
        File.close();
    }

    noecho();
    keypad(stdscr, TRUE);
    use_default_colors();
    start_color();
    setlocale(LC_CTYPE, "");
    
    for(int i = 0; i < getmaxy(stdscr) - 2; i++){
        if(i < mainwininfo.Total_buf.size() - 1){
            mainwininfo.Veiwable_buf.push_back(mainwininfo.Total_buf[i]);
        }
    }

    handle_normal(mainwininfo);
    endwin();
    
}


