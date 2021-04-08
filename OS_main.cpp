#include<iostream>
#include<stdio.h>
#include<string>
#include<string.h>
#include<fstream>
#include <cstdlib>
#include<stdlib.h>

using namespace std;

void LOAD();
int addrmap();
void MOS();
void START_EXECUTION();
void READ();
void WRITE();
void EXECUTE_USERPROGRAM();
void TERMINATE();

char M[300][4]{0}, IR[4], b[40]{'\0'}; // 
char buff1[41] = { '\0' };
char M1[5]{'\0'};
int op1,op2;
int alt[30]{0};
int TI;
int PTR{NULL};

struct PCB
{
    int jobid, ttl, tll;
}p;

char R[4]{'\0'};
int mi,cf;
bool C;
int flagt=0,c=0,ttc=0,llc=0;
string am;
int SI;
int PI;
int IC;
int n;

ifstream infile("input.txt");
ofstream outfile("Output.txt");

//initializing the allocated memory frame
void initPT()
{
    int i;
    for(i=0;i<10;i++)
    {
        M[PTR+i][0]='*';
    }
}

//Allocating a random value from 1 to 30
int random(){
        int n=rand()%30;
        while(alt[n]==1){
            n=rand()%30;
        }
        alt[n]=1;
        return n;
    }

// initializing the process control block
void initPCB(){
    
    // Initializing the process control block 
    p.jobid = (am[3]-48)*1000 + (am[4]-48)*100 + (am[5]-48)*10 + (am[6]-48)*1;
    p.ttl = (am[7]-48)*1000 + (am[8]-48)*100 + (am[9]-48)*10 + (am[10]-48)*1;
    p.tll = (am[11]-48)*1000 + (am[12]-48)*100 + (am[13]-48)*10 + (am[14]-48)*1;
}

// Meh
void TERMINATE()
{
    outfile<<endl;
 	outfile<<endl;
}

// Writing things into the ouput file
void WRITE(){   
        int i,k=0,j;
        i=addrmap();
        //Getting the real address from logical address


        //Storing the information into the buffer from the permanent
        for( j=0;j<40;j++){
            b[j]=M[i][k];
            
            if(k!=3)
                k++;
            
            else{
                k=0;
                i++;
            }
        
        }
        
        // Outputting the lines from the buffer into the output file 
        for(j=0;j<40;j++){
            if(b[j]=='\0'){
                break;
            }
            outfile<<b[j];
        }

        // Re-initialize the buffer
        for(j=0;j<40;j++){
            b[j]='\0';
        }
        outfile<<endl;

    }

// Reading the information from the data card
void READ(){
    char ch;
    int i,k=0,j;
    while(infile){
            infile.get(ch);  // Getting each character
            if(ch=='$'){
                ttc--;   // Decrementing the total time counter
                flagt=1; 
                outfile<<"OUT OF DATA"<<"\nTTL: "<<p.ttl<<"\nTLL: "<<p.tll<<"\nTTC: "<<ttc<<"\nLLC: "<<llc<<"\nIC: "<<IC<<"\nIR: "<<IR[0]<<IR[1]<<IR[2]<<IR[3];
                return;
            }

            if(ch=='\n'){
                break;
            }

            b[c] = ch;  // storing the content from job into the buffer 
            c++;   //size of buffer will be b and it full upto index b-1
            
            //cout<<b<< ":"<<BUFF[b]<<" ";
        }

        // storing the buffer into the main memory 
        i=addrmap(); // getting the real address from the logical address 
        
        for(j=0; j<40; j++){
            
            M[i][k] = b[j];
            
            if(k!=3)
                k++;
            
            else{
                k=0;
                i++;
            }
        }

        // Buffer size strictly limited to 40
        for(j=0; j<40; j++){
            b[j]='\0';
        } // Reinitializing the buffer  

        c=0;//buffer idex initialize to 0
    }

// Page fault thingy
int page_fault(){
    
    // Check for page fault with valid and invalid cases
    char s[2];
    int d1;
    strcpy(s,IR);
    s[2]='\0';
        if(!strcmp(s,"SR")||!strcmp(s,"GD")){
            //cout<<n<<endl;
            
            d1=n/10;
            n=random();
            M[PTR+d1][0] = n/10+48;
            M[PTR+d1][1] = n%10+48;
            IC--;
            PI=0;
            return 1;
        }

        else 
            return 0;
}


// MOS handling
void MOS(){
        int k;
        if(SI!=0)
        {
            if(TI==0) // No time error 
            { // Normal execution of the MOS function
                switch(SI)
                {
                case 1:
                    READ(); // Reading things from the DTA card
                    SI=0; // Re initailizing the system interrupt
                    //cout<<"Read";
                    if(flagt == 1 )
                    {
                        TERMINATE(); // Printing blanks on the file (aesthetic reasons)
                    }
                    break;
                case 2:
                    WRITE();
                    SI=0;
                    // cout<<"Write";
                    break;
                case 3:
                    TERMINATE(); // Putting partitioning lines between 2 programs
                    SI=0;
                    cout<<"Terminated normally";
                    flagt=1;
                    break;
                default:
                    cout<<"ERROR_MOS";
            }

            }
            else if(TI==2) // Time out error
            {
                switch(SI){
                    case 1:// Outputing the registers and the error
                        // TI == 2 , SI == 1 : terminate with time limit exceeded error
                        
                        outfile<<"Time Limit exceeded"<<"\nTTL: "<<p.ttl<<"\nTLL: "<<p.tll<<"\nTTC: "<<ttc<<"\nLLC: "<<llc<<"\nIC: "<<IC<<"\nIR: "<<IR[0]<<IR[1]<<IR[2]<<IR[3];
                        TERMINATE();
                        SI=0;
                        cout<<"Error: Time Limit exceeded";
                        flagt=1;
                        break;

                    case 2:
                        // TI == 2 , SI == 2 : Write and then terminate with time limit exceeded Error

                        WRITE();
                        //outfile<<"Write operation done and Time Limit exceeded ";
                        TERMINATE();
                        SI=0;
                        flagt=1;
                        break;

                    case 3:
                        // TI == 2 , SI == 3 : Terminate normally with no errors
                        //outfile<<"No error"<<endl;
                        TERMINATE();
                        SI=0;
                        flagt=1;
                        break;

                    default:
                        cout<<"ERROR_MOS";
                }
            }
        }
            if(PI!=0){
                if(TI==0){
                    switch(PI){
                        case 1:
                            // Write TI == 0 , PI == 1  : Terminate with operation code error 
                            outfile<<"Operation code error"<<"\nTTL: "<<p.ttl<<"\nTLL: "<<p.tll<<"\nTTC: "<<ttc<<"\nLLC: "<<llc<<"\nIC: "<<IC<<"\nIR: "<<IR[0]<<IR[1]<<IR[2]<<IR[3];
                            TERMINATE();
                            PI=0;
                            flagt=1;
                        break;

                        case 2:
                            // Write TI == 0 , PI == 2  : Terminate with operand error
                            outfile<<"Operand error"<<"\nTTL: "<<p.ttl<<"\nTLL: "<<p.tll<<"\nTTC: "<<ttc<<"\nLLC: "<<llc<<"\nIC: "<<IC<<"\nIR: "<<IR[0]<<IR[1]<<IR[2]<<IR[3];
                            TERMINATE();
                            PI=0;
                            flagt=1;
                        break;

                        case 3:
                        // Write TI == 0 , PI == 3  : If Page Fault Valid, ALLOCATE, update page Table, Adjust IC if necessary,  EXECUTE USER PROGRAM OTHERWISE TERMINATE WITH INVALID PAGE FAULT 
                            cout<<"Page fault function called"<<endl;
                            k=page_fault();

                            // Handling valid page fault error
                            if(k){
                                cout<<"Page fault error handled"<<endl;
                            }

                            // Invalid page fault error
                            else {
                                outfile<<"Page fault error";
                                TERMINATE();
                                flagt=1;
                            }
                        break;

                        default:
                            cout<<"ERROR_MOS";
                    }	
            }

            else if(TI==2){
                switch(PI){
                    case 1:
                        // ti == 2 , PI == 1 : Terminated with time limit exceeded and operation code error
                        outfile<<"Time limit exceeded and opcode error"<<"\nTTL: "<<p.ttl<<"\nTLL: "<<p.tll<<"\nTTC: "<<ttc<<"\nLLC: "<<llc<<"\nIC: "<<IC<<"\nIR: "<<IR[0]<<IR[1]<<IR[2]<<IR[3];
                        TERMINATE();
                        PI=0;
                        cout<<"Time limit exceeded and opcode error";
                        flagt=1;
                    break;
                    
                    case 2:
                        // ti == 2 , PI == 2 : Terminated with time limit exceeded and operand error   
                        outfile<<"Time limit exceeded and operand error "<<"\nTTL: "<<p.ttl<<"\nTLL: "<<p.tll<<"\nTTC: "<<ttc<<"\nLLC: "<<llc<<"\nIC: "<<IC<<"\nIR: "<<IR[0]<<IR[1]<<IR[2]<<IR[3];
                        TERMINATE();
                        PI=0;
                        cout<<"Time limit exceeded and opcode error";
                        flagt=1;
                    break;
                
                    case 3:
                        // TI == 2 , PI == 3 : Terminate with time limit exceeded error
                        TERMINATE();
                        PI=0;
                        outfile<<"Time limit exceeded"<<"\nTTL: "<<p.ttl<<"\nTLL: "<<p.tll<<"\nTTC: "<<ttc<<"\nLLC: "<<llc<<"\nIC: "<<IC<<"\nIR: "<<IR[0]<<IR[1]<<IR[2]<<IR[3];
                        flagt=1;
                    break;
                
                    default:
                        cout<<"ERROR_MOS";
                }
            }
        }

    }


void START_EXECUTION(){
    IC=0; // reinitialzing the instruction counter 
    EXECUTE_USERPROGRAM(); // calling the main user function 
}


// Converting logical to real address
int addrmap(){
    int b;
    // checking for errors 
    if(!isdigit(IR[3])||!isdigit(IR[2])){
       	PI=2;
        MOS();
        return 1;
    }
    
    // Normal execution
    else{
    	n=(IR[3]-48)+(IR[2]-48)*10;
        b=(M[PTR+n/10][0]-48)*10+(M[PTR+n/10][1]-48) ;
        b=b*10;
        return b;
        }
    }


// Checklpoint
void checkcnt(){

    // Checking for time limit counter
    if(p.ttl < ttc){
        TI=2;
    }

    // Checking for line limit counter 
    if(llc > p.tll){
        TI=2;
        ttc--;
        flagt=1;
        outfile<<"Line limit exceeded"<<"\nTTL: "<<p.ttl<<"\nTLL: "<<p.tll<<"\nTTC: "<<ttc<<"\nLLC: "<<llc<<"\nIC: "<<IC<<"\nIR: "<<IR[0]<<IR[1]<<IR[2]<<IR[3];
        TERMINATE();
    }

}


void EXECUTE_USERPROGRAM(){
    int k,i,RA,d1,d2,r,b,j=0,op3,memid;
    char s[2];
    int op4,op5,t1=0;

    while(1)
    {
        if(flagt==1)
            return;

        if(IC<=9){
            d1 = 0;
            d2 = IC;
        }

        else{
            d1 = IC/10;
            d2 = IC%10;
        }
        checkcnt();
        
        if(flagt == 1)
            return;
            
        RA = (M[PTR+d1][0]-48)*10 + (M[PTR+d1][1]-48);
        RA = RA*10;
        RA = RA+d2;

        for(i = 0; i < 4; i++){
            IR[i] = M[RA][i];
        }

        n = (IR[3]-48)+(IR[2]-48)*10;
        IC++;
        // cout<<endl<<IR;
        if((!isdigit(IR[3]) || !isdigit(IR[2])) && IR[0]!='H'){
            PI=2;
            MOS();
            return;
        }

        cout<<endl;
        strcpy(s, IR);
        s[2]='\0';
        
        if(!strcmp(s,"CR")){
            b = addrmap();
            ttc++;
            checkcnt();
            
            if(flagt){
                return;
            }
                
            for(k = 0; k < 4; k++){
                C = 1;
                if(R[k] != M[b][k]){
                    C = 0;
                    break;
                }
            }
        }
        
        else if(!strcmp(s,"LR")){
            b = addrmap();
            ttc++;
            checkcnt();
            
            if(flagt){
                return;
            }
            
			for(k=0;k<4;k++){
                R[k] = M[b][k];
            }

        }

        else if(!strcmp(s,"BT")){
            b=addrmap();

            ttc++;
            checkcnt();
            
            if(flagt){
                return;
            }

            if(C==1){
              IC=n;
            }

        }

        else if(!strcmp(s,"SR")){
            ttc++;
            checkcnt();

            if(M[PTR+n/10][0]=='*'){
                PI=3;
            }

            else{
                b=addrmap();

                    if(flagt){
                        return;
                    }

                
                for(k=0;k<4;k++){
                    M[b][k]=R[k];
                }
            }

        }
        
        else if(!strcmp(s,"GD")){
            ttc++;
            checkcnt();
            
            if(M[PTR+n/10][0]=='*'){
                PI=3;
            }
            
            else{
                    b=addrmap();
                    
                    if(flagt){
                        return;
                    }

                SI=1;
            }
        }

        else if(!strcmp(s,"PD")){
            
            llc++;
            checkcnt();
            
            if(M[PTR+n/10][0]=='*'){
                PI=3;
            }
            
            else{
                ttc++;
            	checkcnt();
            	b=addrmap();
            	
                if(flagt){
                	return;
                }

            SI=2;
            }
        }

        else if(!strcmp(s,"H")){
            ttc++;
            checkcnt();
            SI=3;
        }
        
        else{
            PI=1;
        }

        if(SI!=0 || PI!=0 || TI!=0){
            MOS();
        }

    }
}


// Loading the info from the input file to the main memory
void LOAD(){
    int count = 0;
    int i = 0 , k = 0 , j = 0 , n1 = 0;
    char ch;

    n1 = random();
    
    M[PTR][0] = n1/10+48;
    M[PTR][1] = n1%10+48; /// Converting the int 
    
    i = n1*10;
    
    while(infile){
        
        if(count>9){
            j++;
            n1 = random();
            M[PTR+j][0] = n1/10+48;
            M[PTR+j][1] = n1%10+48;
            count = 0;
            i = n1*10;
        }

        infile.get(ch);
        
        if(ch == 'H'){
            M[i][k] = 'H';
            M[i][k+1] = '\0';
            M[i][k+2] = '\0';
            M[i][k+3] = '\0';
            k = k+3;
            return ;
        }
        
        M[i][k] = ch;
        
        if(k!=3)
            k++;
        
        else{
            k = 0;
            i++;
            count++;
        }
    }
}


int main()
{
    int i,k,j;
    char ch;

    if(!infile){
        cout<<"\n Error in file opening";
        return 1;
    }
    
    
    while(infile)
    {
        infile.get(ch);
        if(ch!='$'){
            // break;
            cout<<"";
        }

        else{
            getline(infile,am);
            if(am.find("AMJ")!=string::npos){
                cout<<am;
                initPCB();
                PTR=rand()%30;
                alt[PTR]=1;
                PTR=PTR*10;
                cout<<"\nThe PTR is located at: M["<<PTR<<"]"<<endl;
                initPT();

                LOAD();
                infile.get(ch);
        }

        else if(am.find("DTA")!=string::npos){
                START_EXECUTION();//slavemode
        }
        
        else if(am.find("END")!=string::npos){       
            for(i=0; i<300; i++){
    	   		cout<<"M["<<i<<"] = "<<M[i][0]<<M[i][1]<<M[i][2]<<M[i][3]<<endl;
			}
                
            for(i=0;i<300;i++){
                for(k=0;k<4;k++){
                    M[i][k]='\0';
                }
               // cout<<"\n";
            }

            for(j=0;j<40;j++){
        		buff1[j]='\0';
    		}
            
            flagt=0;
            IC=0;
            TI=0;
            PI=0;
            SI=0;
            ttc=0;
            llc=0;
            cf=0;

            for(k=0;k<30;k++){
                alt[k]=0;
                }      
            }
        }
    }
        
    return(0);
}
