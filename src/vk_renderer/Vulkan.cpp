//
// Created by FullHat on 10/08/2025.
//
module;
#include <algorithm>
#include <format>
#include <numeric>
#include <unordered_set>
#include <ranges>
#include <cstdint> # do not remove even if it is not in use!
#include <cstring>
#include <format>
#include <vector>
#include <string>

module Vulkan;

import vulkan_hpp;
import Vulkan.Version;

struct VulkanImpl {
    VulkanImpl() = default;

    vk::raii::Instance m_instance { nullptr };
    vk::raii::Context m_context {};
};

Vulkan::Vulkan() : m_this(new VulkanImpl())
{
}

Vulkan::~Vulkan()
{
    delete m_this;
}

template <typename T>
constexpr bool is_string_like_v =
    std::is_same_v<std::decay_t<T>, std::string> ||
    std::is_same_v<std::decay_t<T>, std::string_view> ||
    std::is_same_v<std::decay_t<T>, const char*> ||
    std::is_same_v<std::decay_t<T>, char*>;

void Vulkan::InitInstance()
{
    // input
    std::unordered_set<std::string> requiredExtensions;

    constexpr vk::ApplicationInfo appInfo {
        .sType = vk::StructureType::eApplicationInfo,
        .pApplicationName = "Vulkan",
        .applicationVersion = vk::makeApiVersion(1, 0, 0, 0),
        .pEngineName = "PieEngine",
        .engineVersion = vk::makeApiVersion(1, 0, 0, 0),
        .apiVersion = vk::ApiVersion10,
    };

    vk::InstanceCreateInfo instanceCreateInfo {
        .pApplicationInfo = &appInfo
    };

    // get available extensions
    const auto availableExts = m_this->m_context.enumerateInstanceExtensionProperties();
    // fix for new apple systems
    if (Version(m_this->m_context.enumerateInstanceVersion()) > Version(0, 1, 3, 215))
    {
        requiredExtensions.emplace(vk::KHRPortabilityEnumerationExtensionName);
        instanceCreateInfo.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
    }

    // check if all extensions available
    logger->info("available extensions:");
    for (const auto& [extensionName, specVersion] : availableExts)
    {
        logger->info(std::format("\tn: {}, v: {}",
            extensionName.data(),
            Version(specVersion).ToString()));
    }
    const bool extsAreAvailable = std::ranges::all_of(requiredExtensions, /*are exist in vector*/ [&](const auto additionalExt) {
        return std::ranges::find_if(availableExts, [&](const vk::ExtensionProperties &availableExt) {
            return std::strcmp(availableExt.extensionName.data(), additionalExt.data()) == 0;
        }) != availableExts.end();
    });

    {
#ifdef NDEBUG // debug off
    if (!extsAreAvailable) {
#endif
        // return string in "extName1 [version1], extName2 [version2], .." format
        const auto formatExtensions = [](const auto &exts) {
            std::vector<std::string> parts;
            parts.reserve(std::ranges::distance(exts)); // ok for random-access, else remove reserve

            for (const auto &ext : exts) {
                using el_t = std::decay_t<decltype(ext)>;
                if constexpr (std::is_same_v<el_t, vk::ExtensionProperties>) {
                    parts.push_back(std::format("{} [{}]", ext.extensionName.data(), ext.specVersion));
                } else if constexpr (is_string_like_v<el_t>) {
                    parts.push_back(std::format("{} [not provided]", ext));
                } else {
                    static_assert(std::is_same_v<el_t, void>, "Unsupported extension element type");
                }
            }

            return std::accumulate(parts.begin(), parts.end(), std::string(),
                                   [](std::string acc, const std::string &s) {
                                      return acc.empty() ? s : std::format("{}, {}", acc, s);
                                   });
        };

        const std::string availableExtsS = formatExtensions(availableExts);
        const std::string requiredExtsS = formatExtensions(requiredExtensions);

#ifndef NDEBUG // debug on
        if (!extsAreAvailable) {
#endif
            throw std::runtime_error(std::format(
                "some extensions are not available:\navailable: {},\nrequired: {}",
                availableExtsS, requiredExtsS
            ));
        }
    }

    // unordered_set<string> -> vector<const char *>
    // !!! set must be alive while extsNames is alive
    std::vector<const char*> extsNames;
    extsNames.reserve(requiredExtensions.size());
    for (const auto& requiredExt : requiredExtensions) {
        extsNames.push_back(requiredExt.c_str());
    }
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extsNames.size());
    instanceCreateInfo.ppEnabledExtensionNames = extsNames.data();

    m_this->m_instance = vk::raii::Instance(m_this->m_context, instanceCreateInfo);
    logger->info(std::format("instance created"));
}
