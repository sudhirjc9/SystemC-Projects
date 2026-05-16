#include "systemc.h"

// ─────────────────────────────────────────────
// RAM: SC_METHOD sensitive to 'we'
// Write-enable high  → write data_in to mem[addr]
// Write-enable low   → drive data_out from mem[addr]
// ─────────────────────────────────────────────
SC_MODULE(RAM) {
    sc_in<sc_uint<8>>   addr;
    sc_in<sc_uint<8>>   data_in;
    sc_in<bool>         we;
    sc_out<sc_uint<8>>  data_out;

    sc_uint<8> mem[256];

    void process() {
        if (we.read()) {
            mem[addr.read()] = data_in.read();          // write
            data_out.write(data_in.read());             // write-through
            std::cout << sc_time_stamp()
                      << " | RAM WRITE  addr=0x" << std::hex << addr.read().to_uint()
                      << " data=0x" << data_in.read().to_uint() << std::dec << std::endl;
        } else {
            data_out.write(mem[addr.read()]);           // read
        }
    }

    SC_CTOR(RAM) {
        // zero-initialize memory
        for (int i = 0; i < 256; i++) mem[i] = 0;

        SC_METHOD(process);
        sensitive << we << addr << data_in;
        dont_initialize();
    }
};

// ─────────────────────────────────────────────
// ROM: SC_METHOD sensitive to 'addr'
// Combinationally drives data_out = mem[addr]
// Pre-loaded: mem[i] = i * 2
// ─────────────────────────────────────────────
SC_MODULE(ROM) {
    sc_in<sc_uint<8>>   addr;
    sc_out<sc_uint<8>>  data_out;

    sc_uint<8> mem[256];

    void process() {
        data_out.write(mem[addr.read()]);
        std::cout << sc_time_stamp()
                  << " | ROM  READ  addr=0x" << std::hex << addr.read().to_uint()
                  << " data=0x" << mem[addr.read()].to_uint() << std::dec << std::endl;
    }

    SC_CTOR(ROM) {
        for (int i = 0; i < 256; i++) mem[i] = i * 2;  // pre-load

        SC_METHOD(process);
        sensitive << addr;
        dont_initialize();
    }
};

// ─────────────────────────────────────────────
// MemorySubsystem: parent module
// Owns RAM and ROM, wires them via internal signals
// ─────────────────────────────────────────────
SC_MODULE(MemorySubsystem) {
    // ── External ports (RAM side) ──
    sc_in<sc_uint<8>>   addr;
    sc_in<sc_uint<8>>   data_in;
    sc_in<bool>         we;
    sc_out<sc_uint<8>>  data_out;

    // ── External ports (ROM side) ──
    sc_in<sc_uint<8>>   rom_addr;
    sc_out<sc_uint<8>>  rom_out;

    // ── Submodules ── (initialized in initializer list)
    RAM ram;
    ROM rom;

    // ── Internal signals: external ports → submodule ports ──
    // (direct port-to-port binding not allowed; signals act as wires)
    sc_signal<sc_uint<8>>  sig_addr;
    sc_signal<sc_uint<8>>  sig_data_in;
    sc_signal<bool>        sig_we;
    sc_signal<sc_uint<8>>  sig_data_out;

    sc_signal<sc_uint<8>>  sig_rom_addr;
    sc_signal<sc_uint<8>>  sig_rom_out;

    // ── Pass-through processes: external ports → internal signals ──
    void fwd_ram_inputs() {
        sig_addr.write(addr.read());
        sig_data_in.write(data_in.read());
        sig_we.write(we.read());
    }

    void fwd_ram_output()  { data_out.write(sig_data_out.read()); }
    void fwd_rom_input()   { sig_rom_addr.write(rom_addr.read()); }
    void fwd_rom_output()  { rom_out.write(sig_rom_out.read()); }

    SC_CTOR(MemorySubsystem)
        : ram("ram"), rom("rom")           // ← submodules MUST be named here
    {
        // ── Bind RAM ports to internal signals ──
        ram.addr(sig_addr);
        ram.data_in(sig_data_in);
        ram.we(sig_we);
        ram.data_out(sig_data_out);

        // ── Bind ROM ports to internal signals ──
        rom.addr(sig_rom_addr);
        rom.data_out(sig_rom_out);

        // ── Forward processes ──
        SC_METHOD(fwd_ram_inputs);
        sensitive << addr << data_in << we;

        SC_METHOD(fwd_ram_output);
        sensitive << sig_data_out;

        SC_METHOD(fwd_rom_input);
        sensitive << rom_addr;

        SC_METHOD(fwd_rom_output);
        sensitive << sig_rom_out;
    }
};

// ─────────────────────────────────────────────
// Testbench
// ─────────────────────────────────────────────
int sc_main(int argc, char* argv[]) {
    // ── DUT signals ──
    sc_signal<sc_uint<8>> s_addr, s_data_in, s_data_out;
    sc_signal<bool>       s_we;
    sc_signal<sc_uint<8>> s_rom_addr, s_rom_out;

    // ── Instantiate ──
    MemorySubsystem dut("dut");
    dut.addr(s_addr);
    dut.data_in(s_data_in);
    dut.we(s_we);
    dut.data_out(s_data_out);
    dut.rom_addr(s_rom_addr);
    dut.rom_out(s_rom_out);

    // ─── RAM writes: 0xAA→addr0, 0xBB→addr1, 0xCC→addr2 ───
    auto ram_write = [&](uint8_t a, uint8_t d) {
        s_addr = a; s_data_in = d; s_we = true;
        sc_start(1, SC_NS);
        s_we = false;
        sc_start(1, SC_NS);
    };

    ram_write(0, 0xAA);
    ram_write(1, 0xBB);
    ram_write(2, 0xCC);

    // ─── ROM reads: addr 5, 10, 20 (expect 10, 20, 40) ───
    auto rom_read = [&](uint8_t a) {
        s_rom_addr = a;
        sc_start(1, SC_NS);
        std::cout << sc_time_stamp()
                  << " | TB: ROM[" << (int)a << "] = 0x"
                  << std::hex << s_rom_out.read().to_uint()
                  << std::dec << std::endl;
    };

    rom_read(5);
    rom_read(10);
    rom_read(20);

    return 0;
}