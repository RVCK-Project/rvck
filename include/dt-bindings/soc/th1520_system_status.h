
#ifndef _DT_BINDINGS_SOC_TH1520_SYSTEM_STATUS_H
#define _DT_BINDINGS_SOC_TH1520_SYSTEM_STATUS_H

#define SYS_STATUS_NORMAL (1 << 0)
#define SYS_STATUS_VIDEO_1080P (1 << 1)
#define SYS_STATUS_VIDEO_1080P_60F (1 << 2)
#define SYS_STATUS_VIDEO_2K (1 << 3)
#define SYS_STATUS_VIDEO_2K_60F (1 << 4)
#define SYS_STATUS_VIDEO_4K (1 << 5)
#define SYS_STATUS_VIDEO_4K_60F (1 << 6)
#define SYS_STATUS_VSYNC (1 << 7)
#define SYS_STATUS_DSP (1 << 8)
#define SYS_STATUS_DPU (1 << 9)
#define SYS_STATUS_LAUNCH (1 << 10)
#define SYS_STATUS_PERFORMANCE (1 << 11)

#define DEV_STATUS_OFFSET	12

#define DEV_STATUS_LOW		(1 << (DEV_STATUS_OFFSET + 0))
#define DEV_STATUS_MEDIUM	(1 << (DEV_STATUS_OFFSET + 1))
#define DEV_STATUS_HIGH		(1 << (DEV_STATUS_OFFSET + 2))

#define DEV_STATUS_MASK		(DEV_STATUS_LOW | DEV_STATUS_MEDIUM | DEV_STATUS_HIGH)

#define SYS_STATUS_VIDEO2K_LIMIT (SYS_STATUS_VIDEO_2K | \
					SYS_STATUS_VIDEO_2K_60F)

#define SYS_STATUS_VIDEO4K_LIMIT (SYS_STATUS_VIDEO_4K | \
					SYS_STATUS_VIDEO_4K_60F)

#define SYS_STATUS_VIDEO_LIMIT (SYS_STATUS_VIDEO_1080P_60F | \
					SYS_STATUS_VIDEO2K_LIMIT | \
					SYS_STATUS_VIDEO4K_LIMIT)

#define SYS_STATUS_CPU (SYS_STATUS_LAUNCH)

#endif
