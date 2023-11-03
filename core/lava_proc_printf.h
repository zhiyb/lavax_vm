#pragma once

void LavaProc::lava_op_sprintf(uint32_t ds0)
{
    uint32_t nparams = (uint8_t)ds0 - 2;
    std::vector<uint32_t> params;
    for (int i = 0; i < nparams; i++)
        params.push_back(ram.pop());

    auto const &fmt = ram.readStringData(ram.pop());
    uint32_t dst = ram.pop();

    std::vector<uint8_t> out;
    int iparam = 0;
    for (int i = 0; i < fmt.size(); i++) {
        // Process the format string
        char cfmt = fmt[i];
        if (cfmt == '\0') {
            out.push_back('\0');
            break;
        } else if (cfmt != '%') {
            // Normal character
            out.push_back(cfmt);
            continue;
        }

        // % format specifier
        i++;

        // Check special formatting
        bool left_adjust = false;   // flag 0x80
        bool zero_fill = false;     // flag 0x40
        for (;;) {
            char c = fmt[i];
            if (c == '-')
                left_adjust = true;
            else if (c == '0')
                zero_fill = true;
            else
                break;
            i++;
        }

        // Check field length
        int len = 0;
        for (;;) {
            char c = fmt[i];
            if (c >= '0' && c <= '9')
                len = len * 10 + c - '0';
            else
                break;
            i++;
        }

        // Check format type
        cfmt = fmt[i];
        if (cfmt == 'd') {
            std::string sval = std::to_string((int32_t)params[iparam++]);
            if (len > sval.length()) {
                char cfill = left_adjust ? ' ' : zero_fill ? '0' : ' ';
                std::string sfill(len - sval.length(), cfill);
                if (left_adjust)
                    sval = sfill + sval;
                else
                    sval = sval + sfill;
            }
            out.insert(out.end(), sval.cbegin(), sval.cend());
        } else if (cfmt == 'f') {
            TODO();
            out.push_back(cfmt);
        } else if (cfmt == '%') {
            out.push_back(cfmt);
        } else if (cfmt == 'c') {
            TODO();
            out.push_back(cfmt);
        } else if (cfmt == 's') {
            TODO();
            out.push_back(cfmt);
        } else {
            // Unknown
            out.push_back(cfmt);
        }
    }

    ram.writeStringData(dst, out);
    //std::cerr << __func__ << ": " << dst << ", " << (char *)out.data() << std::endl;
}
