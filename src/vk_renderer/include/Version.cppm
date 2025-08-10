//
// Created by FullHat on 10/08/2025.
//
module;
#include <cstdint>
#include <format>
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
export module Vulkan.Version;

export struct Version
{
    explicit
    Version(const uint32_t fullVersion) : m_version{fullVersion} {}

    Version(const uint32_t variant, const uint32_t major, const uint32_t minor, const uint32_t patch) :
        m_version{VK_MAKE_API_VERSION(variant, major, minor, patch)} {}

    [[nodiscard]]
    uint32_t GetVariant() const { return VK_API_VERSION_VARIANT(m_version); }

    [[nodiscard]]
    uint32_t GetMajor() const { return VK_API_VERSION_MAJOR(m_version); }

    [[nodiscard]]
    uint32_t GetMinor() const { return VK_API_VERSION_MINOR(m_version); }

    [[nodiscard]]
    uint32_t GetPatch() const { return VK_API_VERSION_PATCH(m_version); }

    [[nodiscard]]
    std::string ToString() const { return std::format("{}.{}.{}.{}", GetVariant(), GetMajor(), GetMinor(), GetPatch()); }

    [[nodiscard]]
    bool operator<(const uint32_t otherVersion) const
    {
        const Version Other(otherVersion);
        if (GetVariant() != Other.GetVariant()) { return GetVariant() < Other.GetVariant(); }
        if (GetMajor() != Other.GetMajor()) { return GetMajor() < Other.GetMajor(); }
        if (GetMinor() != Other.GetMinor()) { return GetMinor() < Other.GetMinor(); }
        return GetPatch() < Other.GetPatch();
    }

    [[nodiscard]]
    bool operator>(const uint32_t otherVersion) const
    {
        const Version Other(otherVersion);
        if (GetVariant() != Other.GetVariant()) { return GetVariant() > Other.GetVariant(); }
        if (GetMajor() != Other.GetMajor()) { return GetMajor() > Other.GetMajor(); }
        if (GetMinor() != Other.GetMinor()) { return GetMinor() > Other.GetMinor(); }
        return GetPatch() > Other.GetPatch();
    }

    uint32_t m_version;
};
