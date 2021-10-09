/******************************************************************************
    APB Bus C model
*******************************************************************************/

/******************************************************************************
    APB Bus C model
*******************************************************************************/

#include <stdio.h>

int PCLK;
struct APB{
    unsigned int PADDR;
    unsigned int PWRITE; // Please note that this int should be treated as boolean 0/1
    unsigned int PSEL; // Please note that this int should be treated as boolean 0/1
    unsigned int PENABLE; // Please note that this int should be treated as boolean 0/1
    unsigned int PWDATA;
    unsigned int PRDATA;
    unsigned int PREADY; // Please note that this int should be treated as boolean 0/1
    // This APB memory (ram)
    unsigned int mem[64];
    unsigned int slave_error; // 64 entry ram where each entry is 4 Byte i.e. 256Byte
    // You can define your internal variable here
};

struct APB s_apb;

void init_apb_slave()
{
    s_apb.PADDR   = 0;
    s_apb.PWRITE  = 0;
    s_apb.PSEL    = 0;
    s_apb.PENABLE = 0;
    s_apb.PWDATA  = 0;
    s_apb.PRDATA  = 0;
    s_apb.PREADY  = 0;
    int i;
    for(i=0; i<64; i++)
        s_apb.mem[i] = 0;
}

void write_apb_slave(unsigned int address, unsigned int wdata)
{
    //s_apb.PREADY = 0;

    if(s_apb.PSEL && s_apb.PENABLE && s_apb.PWRITE && address<65)
    {
        s_apb.mem[address] = wdata;
        s_apb.PREADY = 1 ;
    }

    if(!s_apb.PENABLE)
    {
        s_apb.PREADY = 0 ;
    }

    return;
}

void read_apb_slave(unsigned int address)
{
     //s_apb.PREADY = 0;

     if(s_apb.PSEL && s_apb.PENABLE && !s_apb.PWRITE && address<65)
     {
          s_apb.PRDATA = s_apb.mem[address];
          s_apb.PREADY = 1;
     }

      if(!s_apb.PENABLE)
    {
        s_apb.PREADY = 0;
    }

    return;
}

void master_write(unsigned int address, unsigned int wdata)
{
    s_apb.PSEL = 1;
    s_apb.PWRITE = 1;
    s_apb.PADDR = address;
    s_apb.PWDATA = wdata;

}

void master_read (unsigned int address)
{
    s_apb.PSEL = 1;
    s_apb.PWRITE = 0;
    s_apb.PADDR = address;

}

void slave_enable ()
{
    s_apb.PENABLE = 1;
}

void slave_error(unsigned int address)
{
    if(address>=65)
    {
        s_apb.slave_error = 1;
    }
    else
    {
        s_apb.slave_error = 0;
    }
}

void slave_off ()
{
    s_apb.PSEL = 0;
    s_apb.PENABLE = 0;
    s_apb.PWRITE = 0;
    s_apb.PADDR = 0;
    s_apb.PWDATA = 0;
}

void init_dump(FILE *fp)
{
    fprintf(fp, "$version Generated by C APB model $end\n");
    fprintf(fp, "$timescale 1ns $end\n");
    fprintf(fp, "$scope module s_apb $end\n");
    fprintf(fp, "$var wire 1 _PCLK_ PCLK $end\n");
    fprintf(fp, "$var wire 32 _PADDR_ PADDR $end\n");
    fprintf(fp, "$var wire 1 _PWRITE_ PWRITE $end\n");
    fprintf(fp, "$var wire 1 _PSEL_ PSEL $end\n");
    fprintf(fp, "$var wire 1 _PENABLE_ PENABLE $end\n");
    fprintf(fp, "$var wire 32 _PWDATA_ PWDATA $end\n");
    fprintf(fp, "$var wire 32 _PRDATA_ PRDATA $end\n");
    fprintf(fp, "$var wire 1 _PREADY_ PREADY $end\n");
    fprintf(fp, "$var wire 1 _slave_error_ slave_error $end\n");
    fprintf(fp, "$upscope $end\n");
    fprintf(fp, "$enddefinitions $end\n");
}

void u2bin(FILE *fp, int n)
{
    int c;
    for (c = 31; c >= 0; c--)
    {
        int k = n >> c;
        if (k & 1)
            fprintf(fp,"1");
        else
            fprintf(fp,"0");
    }
}

void dump(FILE *fp, int time_stamp)
{
    fprintf(fp,"#%d\n",time_stamp);
    fprintf(fp,"%d_PCLK_\n",PCLK);
    fprintf(fp,"b");
    u2bin(fp, s_apb.PADDR);
    fprintf(fp," _PADDR_\n");
    fprintf(fp,"%d_PWRITE_\n",s_apb.PWRITE);
    fprintf(fp,"%d_PSEL_\n",s_apb.PSEL);
    fprintf(fp,"%d_PENABLE_\n",s_apb.PENABLE);
    fprintf(fp,"b");
    u2bin(fp, s_apb.PWDATA);
    fprintf(fp," _PWDATA_\n");
    fprintf(fp,"b");
    u2bin(fp, s_apb.PRDATA);
    fprintf(fp," _PRDATA_\n");
    fprintf(fp,"%d_PREADY_\n",s_apb.PREADY);
    fprintf(fp,"%d_slave_error_\n",s_apb.slave_error);
}

int main()
{
    int half_cycle_cnt=0;
    //int sim_cy[13];

    PCLK = 0;
    FILE *fp;
    int i;
    int j = 0;
    int tst_addr[3] = {1,2,3};
    int tst_data[3] = {1024,1000,2000};

    static  int sim_cy  [31][3] = {0};
    for (i=5; i<26; i+=4)
    {
        if(i<14)
        {

            sim_cy[i][0]= 1;
            //printf("hello\n");


            if (j>2)
            {
                break;
            }

            sim_cy[i][1]= tst_addr[j];
            sim_cy[i][2]= tst_data[j];


            j++;

        }
        else
        {
            sim_cy[i][0]= 2;
            //printf("hello\n");


             if (j>5)
            {
                break;
            }

            sim_cy[i][1]= tst_addr[j-3];

            j++;

        }

    }

    for(i=6; i<27; i+=4)
    {
        sim_cy[i][0]= 3;
    }

    for(i=0;i<32;i++)
    {
        for(j=0;j<3;j++)
        {
            printf("%d",sim_cy[i][j]);
            printf(" ");
        }

        printf("\n");
    }


    fp = fopen("dump.vcd", "w+");
    if(fp == NULL)
    {
      printf("Error opening dump.vcd!");
      return(-1);
    }
    printf("Begining APB simulation model\n");
    printf("Please check the dump.vcd file for generated waveform (e.g. using gtkwave)\n");
    printf("Initializing Slave model\n");
    init_apb_slave();
    init_dump(fp);
    dump(fp, half_cycle_cnt);
    printf("Slave model Initialized \n");
    printf("Begin Simulation \n");




    for(i=0; i<32; i++)
    {
        half_cycle_cnt = half_cycle_cnt + 5;
        // Positive edge of clk
        PCLK = 1;

        switch(sim_cy[i][0])
       {
    case 0 :
        slave_off();
        break;
    case 1 :

            printf("Call write\n");
            master_write(sim_cy[i][1],sim_cy[i][2]);
            slave_error(sim_cy[i][1]);

        break;
    case 2 :

            printf("Call read\n");
            master_read(sim_cy[i][1]);
            slave_error(sim_cy[i][1]);

        break;
    case 3:
        slave_enable();
        break;
    default:
        slave_off();
        break;
      }
        write_apb_slave(s_apb.PADDR,s_apb.PWDATA);
        read_apb_slave(s_apb.PADDR);

        dump(fp, half_cycle_cnt);
        half_cycle_cnt = half_cycle_cnt + 5;
        // Negative edge of clk
        PCLK = 0;


        dump(fp, half_cycle_cnt);

    }
    fclose(fp);
    return 0;
}



