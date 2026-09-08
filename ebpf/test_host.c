
#include <stdio.h>
#include <string.h>
#include "ebpf_mini.h"
#include "policy_mavlink.h"
#include "policy_conflict.h"

static int failures = 0;

static void jiancha(const char *name, const struct ebpf_prog *p,
                  uint64_t want_ret, uint8_t want_fault) {
    struct ebpf_result r;
    int rc = ebpf_yunxing(p, &r);
    int ok = (r.retval == want_ret) && (r.fault == want_fault) &&
             (rc == (want_fault == EBPF_OK ? 0 : -1));
    printf("%-28s ret=%llu fault=%u insns=%u  期望 ret=%llu fault=%u  %s\n",
           name, (unsigned long long)r.retval, r.fault, r.insns_executed,
           (unsigned long long)want_ret, want_fault, ok ? "PASS" : "FAIL");
    if (!ok) failures++;
}

#define PROG_WITH(data) \
    { POLICY_MAVLINK, POLICY_MAVLINK_CNT, data, (uint32_t)sizeof(data), 1000000, 0, 0 }

int main(void) {
    struct ebpf_prog p = PROG_WITH(PKT_GPI);

    p.ctx = PKT_GPI;      p.ctx_size = sizeof(PKT_GPI);
    jiancha("GPI 合法帧", &p, 1, EBPF_OK);
    p.ctx = PKT_HB;       p.ctx_size = sizeof(PKT_HB);
    jiancha("HEARTBEAT 合法帧", &p, 1, EBPF_OK);
    p.ctx = PKT_BADMAGIC; p.ctx_size = sizeof(PKT_BADMAGIC);
    jiancha("魔数错误", &p, 0, EBPF_OK);
    p.ctx = PKT_BADMSGID; p.ctx_size = sizeof(PKT_BADMSGID);
    jiancha("msgid 不在白名单", &p, 2, EBPF_OK);
    p.ctx = PKT_BIGLEN;   p.ctx_size = sizeof(PKT_BIGLEN);
    jiancha("payload 超长", &p, 3, EBPF_OK);
    p.ctx = PKT_BADLAT;   p.ctx_size = sizeof(PKT_BADLAT);
    jiancha("纬度越界", &p, 4, EBPF_OK);

    {
        static const uint64_t ins[] = { LDXB(2, 1, 100), MOV64I(0, 1), EXIT() };
        struct ebpf_prog q = { ins, 3, PKT_HB, sizeof(PKT_HB), 1000, 0, 0 };
        jiancha("越界读拦截", &q, 0, EBPF_FAULT_MEM);
    }

    {
        static const uint64_t ins[] = { STW(1, 0, 0x11223344), MOV64I(0, 1), EXIT() };
        struct ebpf_prog q = { ins, 3, PKT_HB, sizeof(PKT_HB), 1000, 0, 0 };
        jiancha("写内存拦截", &q, 0, EBPF_FAULT_STORE);
    }

    {
        static const uint64_t ins[] = {
            MOV64I(2, 5), MOV64I(3, 0), DIV64(2, 3), MOV64I(0, 7), EXIT()
        };
        struct ebpf_prog q = { ins, 5, PKT_HB, sizeof(PKT_HB), 1000, 0, 0 };
        jiancha("除零拦截", &q, 0, EBPF_FAULT_DIV0);
    }

    {
        static const uint64_t ins[] = {
            MOV64I(2, 1), SLL64I(2, 40),
            MOV64I(3, 1), SLL64I(3, 40),
            JEQ64(2, 3, 2),                 
            MOV64I(0, 0), EXIT(),
            MOV64I(0, 1), EXIT(),
        };
        struct ebpf_prog q = { ins, 9, PKT_HB, sizeof(PKT_HB), 1000, 0, 0 };
        jiancha("64 位移位等价", &q, 1, EBPF_OK);
    }

    {
        static uint32_t grid[64];
        struct ebpf_prog q = { POLICY_CONFLICT, POLICY_CONFLICT_CNT,
                               PKT_GPI, sizeof(PKT_GPI), 1000000, grid, 64 };
        static const uint64_t want[8] = { 1, 1, 1, 5, 5, 5, 5, 5 };

        memset(grid, 0, sizeof(grid));   
        for (int k = 0; k < 8; k++) {
            static uint8_t pkt[42];
            memcpy(pkt, PKT_GPI, sizeof(PKT_GPI));
            if (k >= 4) {           
                pkt[14] = 0xC2; pkt[15] = 0xBC; pkt[16] = 0xEB; pkt[17] = 0x00;
            }
            q.ctx = pkt; q.ctx_size = sizeof(pkt);
            char name[32];
            snprintf(name, sizeof(name), "冲突筛查 包%d", k);
            jiancha(name, &q, want[k], EBPF_OK);
        }
    }

    {
        struct ebpf_prog q = { POLICY_CONFLICT, POLICY_CONFLICT_CNT,
                               PKT_GPI, sizeof(PKT_GPI), 1000000, 0, 0 };
        jiancha("无地图 CALL 拦截", &q, 0, EBPF_FAULT_CALL);
    }

    printf("\n%s（失败 %d 项）\n", failures ? "!!! 存在失败" : "全部通过", failures);
    return failures ? 1 : 0;
}
