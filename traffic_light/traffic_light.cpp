#include <systemc>

using namespace sc_core;
using namespace sc_dt;  

SC_MODULE(traffic_light_controller){
    sc_in<bool> clk;

    sc_out<bool> red, green, yellow;

    enum State { RED_ST, GREEN_ST, YELLOW_ST };
    State    current_state;
    sc_event ev;

    SC_CTOR(traffic_light_controller){
        SC_THREAD(run);
        sensitive <<clk.pos();
        dont_initialize();

        SC_METHOD(print);
        dont_initialize();
        sensitive<<ev;
    }

     // Helper: drive all three outputs atomically
    void set_outputs(bool r, bool g, bool y) {
        red.write(r);
        green.write(g);
        yellow.write(y);
    }


    void run(){

        // Intial state is RED
            set_outputs(true, false, false);
            current_state = RED_ST;
            ev.notify();

 
        while (true) {
            // RED: hold for 3 clock cycles
            for (int i = 0; i < 3; i++) wait();
            set_outputs(false, true, false);
            current_state = GREEN_ST;
            ev.notify();

            // GREEN: hold for 3 clock cycles
            for (int i = 0; i < 3; i++) wait();
            set_outputs(false, false, true);
            current_state = YELLOW_ST;
            ev.notify();

            // YELLOW: hold for 1 clock cycle
            wait();
            set_outputs(true, false, false);
            current_state = RED_ST;
            ev.notify();
        }
        
    }

    void print(){
        switch (current_state) {
            case RED_ST:
                std::cout << sc_time_stamp() << " | RED" << std::endl;
                break;
            case GREEN_ST:
                std::cout << sc_time_stamp() << " | GREEN" << std::endl;
                break;
            case YELLOW_ST:
                std::cout << sc_time_stamp() << " | YELLOW" << std::endl;
                break;
        }
    }
};

int sc_main(int argc, char** argv) {
    traffic_light_controller tl("tl");

    sc_clock clk("clk", 1, SC_SEC);
    tl.clk(clk);

    sc_signal<bool> s_red, s_yellow, s_green;
    tl.red(s_red);
    tl.yellow(s_yellow);
    tl.green(s_green);

    sc_start(30, SC_SEC);
    return 0;
}

