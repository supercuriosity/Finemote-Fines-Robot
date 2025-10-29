/*******************************************************************************
* Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_DOUBLE_BUFFER_HPP
#define FINEMOTE_DOUBLE_BUFFER_HPP

#include <array>

template <size_t N>
class DoubleBuffer {
public:
    DoubleBuffer(std::function<void(uint8_t *, size_t)> decodeCallback): DecodeCallback(decodeCallback) {}

    uint8_t *&GetBuffer() {
        return currentBuffer;
    }

    /**
     *
     * @param size 有效数据长度，0代表默认
     * @note 先重新开始接收，后调用
     * @note (指针指向 Buffer 1) -> Buffer 0 接收完成 -> Buffer 1 开始接收 -> Switch (指针指向 Buffer 0) -> Buffer 0 decode
     */
    void CommitBuffer(size_t size) {
        SwitchBuffer();
        DecodeCallback(currentBuffer, size);
    }

protected:
    void SwitchBuffer() {
        if (currentBuffer == Buffers[0].data()) {
            currentBuffer = Buffers[1].data();
        } else {
            currentBuffer = Buffers[0].data();
        }
    }

private:
    std::array<std::array<uint8_t, N>, 2> Buffers = {};
    uint8_t *currentBuffer = Buffers[0].data();
    std::function<void(uint8_t *, size_t)> DecodeCallback;
};

#endif
