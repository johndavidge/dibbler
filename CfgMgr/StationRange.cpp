/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#include "StationRange.h"
#include "DHCPConst.h"
#include "Logger.h"

TStationRange::TStationRange( SPtr<TDUID> duidl, SPtr<TDUID> duidr)
    :isAddrRange_(false), DUIDL_(duidl), DUIDR_(duidr), PrefixLength_(-1)
{
}

TStationRange::TStationRange( SPtr<TIPv6Addr> addrl, SPtr<TIPv6Addr> addrr)
    :isAddrRange_(true), AddrL_(addrl), AddrR_(addrr), PrefixLength_(-1)
{
    /// @todo: prefix length could be calculated automatically here
}

bool TStationRange::in(SPtr<TDUID> duid, SPtr<TIPv6Addr> addr) const
{
    if (isAddrRange_)
    {
        if ((*addr<=*AddrL_)&&(*AddrL_<=*addr)) return true;
        if ((*addr<=*AddrR_)&&(*AddrR_<=*addr)) return true;
        if (*addr<=*AddrL_) return false;
        if (*AddrR_<=*addr) return false;
    }
    else
    {
        if (!duid)
            return false;
        if (*duid <= *DUIDL_) return false;
        if (*DUIDR_ <= *duid) return false;
        return true;
    }
    return false; // should not happen
}

bool TStationRange::in(SPtr<TIPv6Addr> addr) const
{
    if (isAddrRange_)
    {
        if ((*addr<=*AddrL_)&&(*AddrL_<=*addr)) return true;
        if ((*addr<=*AddrR_)&&(*AddrR_<=*addr)) return true;
        if (*addr<=*AddrL_) return false;
        if (*AddrR_<=*addr) return false;
        return true;
    } else
        return false;
}

bool TStationRange::in(SPtr<TDUID> duid) const
{
    if (isAddrRange_)
        return false;
    else
    {
        if (*duid<=*DUIDL_) return false;
        if (*DUIDR_<=*duid) return false;
    }
    return true;
}

SPtr<TIPv6Addr> TStationRange::getRandomAddr() const  {
    if(isAddrRange_)
    {
        SPtr<TIPv6Addr> diff = new TIPv6Addr();
        *diff=(*AddrR_)-(*AddrL_);
        --(*diff);
        *diff=*diff+*AddrL_;
        return diff;
    }
    else
        return 0;
}

SPtr<TIPv6Addr> TStationRange::getRandomPrefix() const {
    if(isAddrRange_)
    {
        SPtr<TIPv6Addr> diff = new TIPv6Addr();
        *diff=(*AddrR_)-(*AddrL_);
        --(*diff);
        *diff=*diff+*AddrL_;
        return diff;
    }
    else
        return 0;
}



unsigned long TStationRange::rangeCount() const {
    if(isAddrRange_) {
        SPtr<TIPv6Addr> diff(new TIPv6Addr());
        *diff=(*AddrR_)-(*AddrL_);
        char *addr=diff->getAddr();
        for(int i=0;i<12;i++)
        {
            if (addr[i]>0) return DHCPV6_INFINITY;
        }
        unsigned long retVal = addr[12]*256*256*256 + addr[13]*256*256 + addr[14]*256 + addr[15];
        if (retVal!=DHCPV6_INFINITY)
            return retVal + 1;
        return retVal;
    }
    else
        return 0;
}

int TStationRange::getPrefixLength() const {
    return PrefixLength_;
}

void TStationRange::setPrefixLength(int len) {
    PrefixLength_ = len;
}


TStationRange::~TStationRange(void) {
}

SPtr<TIPv6Addr> TStationRange::getAddrL() const {
    return AddrL_;
}

SPtr<TIPv6Addr> TStationRange::getAddrR() const {
    return AddrR_;
}

void TStationRange::truncate(int minPrefix, int maxPrefix) {
    if (!isAddrRange_) {
        Log(Error) << "Unable to truncace this pool: this is DUID pool, not address pool." << LogEnd;
        return;
    }
    // if the L and R addresses are not at the prefix boundaries, then we are
    // pretty f%%%ed up
    AddrL_->truncate(minPrefix, maxPrefix);
    AddrR_->truncate(minPrefix, maxPrefix);
}

std::ostream& operator<<(std::ostream& out, TStationRange& range)
{
    if (range.isAddrRange_) {
        // address range
        if(range.AddrL_&&range.AddrR_)
            out << "      <range type=\"addr\" min=\"" << *range.AddrL_
                << "\" max=\""  << *range.AddrR_ << "\"/>" << std::endl;
    } else {
        // DUID range
        if (range.DUIDL_&&range.DUIDR_) {
            out << "      <range type=\"duid\">" << std::endl
                << "         " << *range.DUIDL_
                << "         " << *range.DUIDR_
                << "      </range>" << std::endl;
        }
    }
    return out;
}
