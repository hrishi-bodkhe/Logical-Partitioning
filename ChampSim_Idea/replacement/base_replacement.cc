#include "cache.h"

uint32_t CACHE::find_victim(uint32_t cpu, uint64_t instr_id, uint32_t set, const BLOCK *current_set, uint64_t ip, uint64_t full_addr, uint32_t type)
{
    // baseline LRU replacement policy for other caches 
    return lru_victim(cpu, instr_id, set, current_set, ip, full_addr, type); 
}

void CACHE::update_replacement_state(uint32_t cpu, uint32_t set, uint32_t way, uint64_t full_addr, uint64_t ip, uint64_t victim_addr, uint32_t type, uint8_t hit)
{
    if (type == WRITEBACK) {
        if (hit) // wrietback hit does not update LRU state
            return;
    }

    return lru_update(set, way);
}

uint32_t CACHE::lru_victim_new(uint32_t cpu, uint64_t instr_id, uint32_t set, const BLOCK *current_set, uint64_t ip, uint64_t full_addr, uint32_t type,
                               uint32_t inst_way_start, uint32_t inst_way_end, uint32_t data_way_start, uint32_t data_way_end, uint32_t line_type)
{
    uint32_t way;

    if(line_type == 0){     //for instructions
        way = inst_way_start;

        // fill invalid line first
        while(way != data_way_start) {

            if (block[set][way].valid == false) {

                DP ( if (warmup_complete[cpu]) {
                    cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " invalid set: " << set << " way: " << way;
                    cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[set][way].address << " data: " << block[set][way].data;
                    cout << dec << " lru: " << block[set][way].lru << endl; });

                break;
            }

            way = (way + 1) % NUM_WAY;
        }

        // LRU victim
        if (way == data_way_start) {

            way = inst_way_start;
//            cout << set << " " << way << "," << inst_way_start << " d-start " << data_way_start << '\n';
            while (way != data_way_start) {
//                cout << way << ":" << block[set][way].lru << ":" << LLC_IWAYS-1 << ',';
                if (block[set][way].lru == LLC_IWAYS-1) {

                    DP ( if (warmup_complete[cpu]) {
                        cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " replace set: " << set << " way: " << way;
                        cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[set][way].address << " data: " << block[set][way].data;
                        cout << dec << " lru: " << block[set][way].lru << endl; });

                    break;
                }
                way = (way + 1) % NUM_WAY;
            }
//            cout << '\n';
        }


        if (way == data_way_start) {
            
            cerr << "[" << NAME << "] " << __func__ << " for instr ways, no victim! set: " << set << endl;
            assert(0);
        }
    } else{ //for data lines
        way = data_way_start;


        // fill invalid line first
        while(way != inst_way_start) {
            if (block[set][way].valid == false) {

                DP ( if (warmup_complete[cpu]) {
                    cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " invalid set: " << set << " way: " << way;
                    cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[set][way].address << " data: " << block[set][way].data;
                    cout << dec << " lru: " << block[set][way].lru << endl; });

                break;
            }

            way = (way + 1) % NUM_WAY;
        }

        // LRU victim
        if (way == inst_way_start) {
//            cout << way << "," << data_way_start << '\n';
            way = data_way_start;
            while (way != inst_way_start) {
//                cout << way << ":" << block[set][way].lru << ":" << LLC_DWAYS-1 << ',';
                if (block[set][way].lru == LLC_DWAYS-1) {
                    DP ( if (warmup_complete[cpu]) {
                        cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " replace set: " << set << " way: " << way;
                        cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[set][way].address << " data: " << block[set][way].data;
                        cout << dec << " lru: " << block[set][way].lru << endl; });

                    break;
                }
                way = (way + 1) % NUM_WAY;
            }
//            cout << '\n';
        }

        if (way == inst_way_start) {
//            for (way=0; way<NUM_WAY; way++){
//                cout << way << ":" << block[set][way].lru << ",";
//            }
//            cout << '\n';

            cerr << "[" << NAME << "] " << __func__ << "for data ways, no victim! set: " << set << endl;
            assert(0);
        }
    }

    return way;
}


uint32_t CACHE::lru_victim(uint32_t cpu, uint64_t instr_id, uint32_t set, const BLOCK *current_set, uint64_t ip, uint64_t full_addr, uint32_t type)
{
    uint32_t way = 0;

    // fill invalid line first
    for (way=0; way<NUM_WAY; way++) {
        if (block[set][way].valid == false) {

            DP ( if (warmup_complete[cpu]) {
            cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " invalid set: " << set << " way: " << way;
            cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[set][way].address << " data: " << block[set][way].data;
            cout << dec << " lru: " << block[set][way].lru << endl; });

            break;
        }
    }

    // LRU victim
    if (way == NUM_WAY) {
        for (way=0; way<NUM_WAY; way++) {
            if (block[set][way].lru == NUM_WAY-1) {

                DP ( if (warmup_complete[cpu]) {
                cout << "[" << NAME << "] " << __func__ << " instr_id: " << instr_id << " replace set: " << set << " way: " << way;
                cout << hex << " address: " << (full_addr>>LOG2_BLOCK_SIZE) << " victim address: " << block[set][way].address << " data: " << block[set][way].data;
                cout << dec << " lru: " << block[set][way].lru << endl; });

                break;
            }
        }
    }

    if (way == NUM_WAY) {
        cerr << "[" << NAME << "] " << __func__ << " no victim! set: " << set << endl;
        assert(0);
    }

    return way;
}

void CACHE::lru_update_new(uint32_t set, uint32_t way, uint32_t instr_start_way, uint32_t instr_end_way,
                           uint32_t data_start_way, uint32_t data_end_way, uint32_t line_type)
{
    if(line_type == 0){ //for instruction ways
        // update lru replacement state
        uint32_t i = instr_start_way;
        while(i != data_start_way) {
            if (block[set][i].lru < block[set][way].lru) {
         
                block[set][i].lru++;
            }


            i = (i + 1) % NUM_WAY;
        }
        block[set][way].lru = 0; // promote to the MRU position
    } else{ // for data ways

        
        // update lru replacement state
        uint32_t i = data_start_way;
        while(i != instr_start_way) {
            if (block[set][i].lru < block[set][way].lru) {
                block[set][i].lru++;
            }

            i = (i + 1) % NUM_WAY;
        }
        block[set][way].lru = 0; // promote to the MRU position
    }
}


void CACHE::lru_update(uint32_t set, uint32_t way)
{
    // update lru replacement state
    for (uint32_t i=0; i<NUM_WAY; i++) {
        if (block[set][i].lru < block[set][way].lru) {
            block[set][i].lru++;
        }
    }
    block[set][way].lru = 0; // promote to the MRU position
}

void CACHE::replacement_final_stats()
{

}

#ifdef NO_CRC2_COMPILE
void InitReplacementState()
{
    
}

uint32_t GetVictimInSet (uint32_t cpu, uint32_t set, const BLOCK *current_set, uint64_t PC, uint64_t paddr, uint32_t type)
{
    return 0;
}

void UpdateReplacementState (uint32_t cpu, uint32_t set, uint32_t way, uint64_t paddr, uint64_t PC, uint64_t victim_addr, uint32_t type, uint8_t hit)
{
    
}

void PrintStats_Heartbeat()
{
    
}

void PrintStats()
{

}
#endif
