#include <systemc>
using namespace sc_core;
using namespace sc_dt;  

SC_MODULE(producer){

    sc_in<bool>         clk;
    sc_fifo_out<int>    out;

    SC_CTOR(producer){
        SC_THREAD(produce);
        sensitive<<clk.pos();

    }

    void produce(){
        wait();
        for(int i=1; i<=10; i++){
            out.write(i);
            std::cout<<sc_time_stamp()<<" Producer wrote: "<<i<<std::endl;
            wait();
            
        }
        std::cout << sc_time_stamp() << " | Producer done." << std::endl;
    }
};

SC_MODULE(consumer){

    sc_in<bool> clk;
    sc_fifo_in<int> in;

    SC_CTOR(consumer){
        SC_THREAD(consume);
        sensitive<<clk.pos();
    }

    void consume(){

        wait();
        while(true){
        int value = in.read();
        std::cout<<sc_time_stamp()<<" Consumer read: "<<value<<std::endl;
        for(int i=0; i<2; i++)
            wait();
        }
    }
};


int sc_main(int argc, char* argv[]){
    sc_fifo<int> f(4);

    producer prod("producer");
    consumer cons("consumer");

    prod.out(f);
    cons.in(f);

    sc_clock clk("clk", 1, SC_NS);

    prod.clk(clk);
    cons.clk(clk);

    sc_start(100, SC_NS);
    return 0;
}