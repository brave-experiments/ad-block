/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef LISTS_REGIONS_H_
#define LISTS_REGIONS_H_

#include <vector>
#include "../filter_list.h"

const std::vector<FilterList> region_lists = {
  FilterList({
    "9FCEECEC-52B4-4487-8E57-8781E82C91D0",
    "https://easylist-downloads.adblockplus.org/Liste_AR.txt",
    "ARA: Liste AR",
    {"ar"},
    "https://forums.lanik.us/viewforum.php?f=98",
    "gpgegghiabhggiplapgdfnfcmodkccji",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAnbHn298ZQjKnWlC6NgkvS3Dr7Neu87d1h8s3b9GTlc1QNDWiYgY5IfWVq/1FBw2nUFE/v8fNJg8quq8Z2nS8dYiJDVSGRggiCooa0OTCARL0BsGxHZO6s2QROYIcxPVnzISqg5zRIBc+8npE68uVUrDR6q/KdJ8siL2hrR/NybPp+uTK44lHOEIBFm8ih1rC6z+Y5dHfhax0CuL6wlWwVNcFe1macYEcOXShwkUOADh6rEBQZKJmv474xJutmB8nIpGq7C2Hn2HNNyfA6tYmhVlsaeEC44phGITKDai03wFsWWkHQPEU5HwFzKQGIBFwudyO8iigO5m+d3XSzgSZtQIDAQAB"
  }), FilterList({
    "FD176DD1-F9A0-4469-B43E-B1764893DD5C",
    "https://stanev.org/abp/adblock_bg.txt",
    "BGR: Bulgarian Adblock list",
    {"bg"},
    "https://stanev.org/abp/",
    "coofeapfgmpkchclgdphgpmfhmnplbpn",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAoqpe6QWKketr66AW+hKSxr5qds9gxmU72N20bG/nG8wfbPUdTwEqIG3m5e37Iq1FI0gcQir5UqwZZUgkum5dWJwgCB1SOaVvlZrWZbTbATKFcePswHqAIXMfS+wzMA/Ifoky26dE4k/rs6J/zQEbeXXon/ikjGJ7GxYeHYMBz9DAPQhcUoBlh1C0O0vhvXU+kG5DO4wdIt9W/cXJtv+8OTZ6HiTJw1j0jAliFZI/jhkYB6MW57OBpBYlWJQhMbLbK5opXq6d4ELbjC1amqI1lT3j5bl0g1OpMqL4Jtz6578G79gMJfxE3hA5tL0rGU3vAmwck/jXh7uOOzqetwdBcwIDAQAB"
  }), FilterList({
    "11F62B02-9D1F-4263-A7F8-77D2B55D4594",
    "https://easylist-downloads.adblockplus.org/easylistchina.txt",
    "CHN: EasyList China (中文)",
    {"zh"},
    "http://abpchina.org/forum/forum.php",
    "llhecljkijgcaalnbfadljdpkpbehakp",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAyahWbgHuWAI7CkBdclxOlehPVVGuG8u6bPi1vs016Kbhn9GThEVIP5qzAFQLA3jRrGy5B2nncdaCnibf7BkGsNR7nyQQuXAI2FGk9qCm36ZF7FI/yjtN0S0e6LzSswOcVhTdPnVkxYY6UDuKyzVRxgbF9Yg1aT45NFpJFZKFtKHexnLiY6KlZKV6GhY1jucjo7W77xdpLaspkYbQ69UvDlSA093InAzzikuqBdKvY0FPvC6pgiefqWTMa4M1cZU9IoIiukqrpXQn1tC9PJ8CU4XKCTshaNbpX5wxY10rUl7i/WHNcXCfmCXxKbqRZ1SyH6KiiBrDpSnfKXxrQip4GwIDAQAB"
  }), FilterList({
    "CC98E4BA-9257-4386-A1BC-1BBF6980324F",
    "https://raw.githubusercontent.com/cjx82630/cjxlist/master/cjx-annoyance.txt", // NOLINT
    "CHN: CJX's Annoyance List",
    {},
    "https://github.com/cjx82630/cjxlist",
    "llpoppgpcimnmhgehpipdmamalmpfbjd",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAudVtKg3tZbgealGvVzbEL3yP1YWdt6GRDreqy/b3kCce3AZ8WroL8jb6Zj/aapBRCNxBezXzij+b6QiIH/l7sn5Wf5HDs5Vnrx4fDvGRtSLpgP0cSuFGVDx71TQz4X+AnUubOeHskIlJJAT4t4cHWs9c7EAl3ShG7DtvL2qHG2TUfJFqYOMOtQd2qG5H+X9zAUFP/qRHT55gzce8h+SXCsvdK4B8XK1cdvbIykllbGPzZr/TANn9gCtMKxUfk1qFn1uYD6mzg80KJmof8MHbLon6KLMqywcqfwEwvoivxo6f5LkOUjhqDYZEQ5la3h7lFfHKz7fCE7FCww7bQ028lwIDAQAB"
  }), FilterList({
    "92AA0D3B-34AC-4657-9A5C-DBAD339AF8E2",
    "https://raw.githubusercontent.com/cjx82630/cjxlist/master/cjxlist.txt",
    "CHN: CJX's EasyList Lite (main focus on Chinese sites)",
    {},
    "https://github.com/cjx82630/cjxlist",
    "lgfeompbgommiobcenmodekodmdajcal",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAtAMyjMBZCbqNIuO01ZFJ5iKmcNFuJHXIUqhO9s6j6XnBAfak/OOk4s9k3maXyhaynXVpYATQyRHR0OEpmsQawFgKCmVm1LB68jxJ5Hh1ZITG1UyfznYnozkjBtzdkMGKeuZFBaHo5PPueHVO7yJDHvU3UFW4vCJ01twXiH4y0qaYjL1CPr58J9U0oKxptsfwEC53WcDq6mKtAKRpyxN6vbtFJ5/li2yC0Ms+8Xe3Xv5ovniM/4vNf3Jn1w0jzgrDRcW2VhxpydsH6q7oaR2igIzJ+XG6/k0g29CJhfT85dJNF31TwqvoI+Ju6hjZrEmSHmC7gbY7gN3ak+DbUrQxjwIDAQAB"
  }), FilterList({
    "7CCB6921-7FDA-4A9B-B70A-12DD0A8F08EA",
    "https://raw.githubusercontent.com/tomasko126/easylistczechandslovak/master/filters.txt", // NOLINT
    "CZE, SVK: EasyList Czech and Slovak",
    {"cs"},
    "https://github.com/tomasko126/easylistczechandslovak",
    "omkkefoeihpbpebhhbhmjekpnegokpbj",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuYBXbfloR5HddFlg80U8+pf5TqfFJQAf1bL4myp9KfGggwqrjuRzIkPOD8J8IvCp8tWv2f4QK9sAPHhtV6w1cnYX24lKxrQ/lHHAV6/CEcFa+2Yk7cRLKDC10H3r4FMRoCeAy/ruTjVPfIw+GuAfFYl1qYWBNxvW7XXw7cCIIYL4j82YQF6HjsWbTT+QHLCR6h66wvIyVQC9ppjJPxDaEevjt4tohEFAB1NBC+Wxt8H/P5r5ayNcLnb9Ygt75haYL8VWZOJhO/neSTyuidTFG5ox2Ruc6TXP8t0IqpVtiZUDkx1jzUakIHoKNMBc7oz3P/SQ4AanZsIliJobXFeUiQIDAQAB"
  }), FilterList({
    "E71426E7-E898-401C-A195-177945415F38",
    "https://easylist-downloads.adblockplus.org/easylistgermany.txt",
    "DEU: EasyList Germany",
    {"de"},
    "https://forums.lanik.us/viewforum.php?f=90",
    "jmomcjcilfpbaaklkifaijjcnancamde",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA1HeinhQW1+SS5Xkfxb1KGAdSQqcm8bTx3fg40xVwsSre6B/VKnBLgOD9mTnbBHBY4rfymxwhD7dTHf05Y7LAkAhTEFLyi09GYPBesJ7uO2EZMYuPEd6iKx1lKo/zF9eO0VrDtjz+vw9zwriHtFMJLxz2+QXH18tx/jcvRiM8mxKB64ma+mZO38zHDs+KPDNBigtXcMhjfKbk3vnl/bl/Adzibx44gEol+abEHkvItLwdaBb3vRTEFiiO8MoTJYFY8qMxwZK4aSr6Ox5yDKzCc7dy/eIarb+zfzbk7QJD+FsNuQ34h/7gMnUE+AX+SNxFpGKRRAJFHbBPr/ofHxorUQIDAQAB"
  }), FilterList({
    "9EF6A21C-5014-4199-95A2-A82491274203",
    "https://adblock.dk/block.csv",
    "DNK: Schacks Adblock Plus liste",
    {"da"},
    "https://henrik.schack.dk/adblock/",
    "facajiciiepdpjnoifonbfgcnlbpbieo",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAxvmMjXthrTjGl0WDrL+4qYSVao5vKPGerr2VeUiZ1o94+P0IJyZzq3d7UP2nOvhveGl15YYxYss+sD/sUkUW57XMx+H4TF5OzGCwV8nkz4VoMIfEU6CKgYmRGHV2VoMdIHG7R++jX20+GAoeBw+aBx9+AHlBouf1kvqbkutVh+Bre1cVa6YsgsPVcmhiEp7wjz2yB23f44+pBIQgWlKWn7z9e1osG4LUCGk6gavtRoNGS3TAUf1Sq9EUibFJVmBjujVoiQKD8GIFKmLM9Fxl1Q+xgG2PCCSBz5lSesHkphDpwhszedurpKbWsnsRPqbqR3GmpceKQheWL/Y56tf2gwIDAQAB"
  }), FilterList({
    "0783DBFD-B5E0-4982-9B4A-711BDDB925B7",
    "https://adblock.ee/list.php",
    "EST: Eesti saitidele kohandatud filter",
    {"et"},
    "https://adblock.ee/",
    "fnpjliiiicbbpkfihnggnmobcpppjhlj",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAnrl1tavPfozqu7CmqNfVUtZfUlIitbWpFBRn+HVW0oEFUNqAwNlwHqy9QZP88wKvb5N3EJj6NAq6je4ii6nMkDn59teNzGA4m8QSkeOWT6pNm98FZA6HNHPnhnYSG2sT8tpQ8Uyh4ySrxj2ijVM0Hc01WKQ6zjkvZWOuZWllsCejRZmxGOLUUy5mtKhIfHiuleZ7AmKx46AiVFvrpvV5x8G2HKAlF/uDc6LmV0lfXcROt5RlY+kD/sQ6wKcatibpHbLoRHOJx3ac13+pvt85773af0MdrvdCYjxvqn3DJlKw9qqk/B59n+XdTmWcfC9k77Z0teoMM5EBy8G1nGbelwIDAQAB"
  }), FilterList({
    "5E5C9C94-0516-45F2-9AFB-800F0EC74FCA",
    "https://raw.githubusercontent.com/liamja/Prebake/master/obtrusive.txt",
    "EU: Prebake - Filter Obtrusive Cookie Notices",
    {},
    "https://github.com/liamja/Prebake",
    "abeicfkepbhgindohkebelkkhnnijcaf",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAsDGkXlpuILNJ/iPEsXXdw+ZVBPf7L11UkG2az6NaeumxKfOTBDivPCXz48lWG4d6jySR8n+eMOaqX8FUVc50AGnWfq/Ud7PJdkvYNEyTNjZw7yQUta3q1yfc5gVsc1IiG9s0GuZxzSWnI0zfwMIG6JNTZTdPeIL1VxnFySXwyYKr1QhkVcpiN5AYNA8jXmZmmZ716Cti+1kAlGLQSO0ABqFZhJvSMGUb+12z6BW20tc4spt8QgY6a8CFMasdoD3hz+gKo+rvQlgPGWQhWxuTgRJZQnyS3EKeJYBaaS11c3xfIqkK85VXuMt9x/3bu9NkXFpXcNwqBo9zs+gxodVSgwIDAQAB"
  }), FilterList({
    "1C6D8556-3400-4358-B9AD-72689D7B2C46",
    "https://raw.githubusercontent.com/finnish-easylist-addition/finnish-easylist-addition/master/Finland_adb.txt",
    "FIN: Finnish Addition to Easylist",
    {"fi"},
    "https://github.com/finnish-easylist-addition/finnish-easylist-addition",
    "kdcalgmhljnckmnfcboeabeepgnlaemf",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA3seBXoyYSdtiqNAIaS5v9jP6Pr8xqgFnZyHknxNsC92fHyRW2nbuwMr78pWA4vPIyV6BFG5jS8k2RXEbWiOKNNsw7nWlfT4QMwkEu4uU1vqxsNDtdc1rdrc69aBegyNOQBS+W6aP1ESHp68AoalYKMHKpc+fi00sdQwYU9Y5oW9q4uRX3baAyuGZjP0xuKN3t+T1QnhbhkldP2WP0ooU/VRMhy2rYoE+W6eQRGrghJJG/wWznz5AiPD9EpPST/hoVWOKVco+12IbdILw7yGX2c65xPcLr6obVR+549QrgxU0W02XxS2lXKGc1NT2Zdl6ugh6XpW1RHVz7SjLIZgifwIDAQAB"
  }), FilterList({
    "9852EFC4-99E4-4F2D-A915-9C3196C7A1DE",
    "https://easylist-downloads.adblockplus.org/liste_fr.txt",
    "FRA: EasyList Liste FR",
    {"fr"},
    "https://forums.lanik.us/viewforum.php?f=91",
    "emaecjinaegfkoklcdafkiocjhoeilao",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAsbqIWuMS7r2OPXCsIPbbLG1H/d3NM9uzCMscw7R9ZV3TwhygvMOpZrNp4Y4hImy2H+HE0OniCqzuOAaq7+SHXcdHwItvLKtnRmeWgdqxgEdzJ8rZMWnfi+dODTbA4QvxI6itU5of8trDFbLzFqgnEOBk8ZxtjM/M5v3UeYh+EYHSEyHnDSJKbKevlXC931xlbdca0q0Ps3Ln6w/pJFByGbOh212mD/PvwS6jIH3LYjrMVUMefKC/ywn/AAdnwM5mGirm1NflQCJQOpTjIhbRIXBlACfV/hwI1lqfKbFnyr4aPOdg3JcOZZVoyi+ko3rKG3vH9JPWEy24Ys9A3SYpTwIDAQAB"
  }), FilterList({
    "6C0F4C7F-969B-48A0-897A-14583015A587",
    "https://www.void.gr/kargig/void-gr-filters.txt",
    "GRC: Greek AdBlock Filter",
    {"el"},
    "https://github.com/kargig/greek-adblockplus-filter",
    "pmgkiiodjlmmpimpmphjhkodjnjfkeke",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4KGRug8Rw1WHk1BPfIdtdw7uwFijUac7jk1lb99lEfSq2uPV2bKCk8lLh/6ahlV/EjSN8mGiFfZIVTDFhuYhVuIO8iETrCZe1ChoI0F8ptHOPQXVPzKUFMpkRqAnH51vqx+3gG78A3+iGfAE+LjerP1j4Jx5jSvTkbN8l+RqKMtjaaL9qRHv3aRQtYB/shGgdxKeOR0f8E6yJ4tIRDHB72bDufN7wbnRoHCNnLkrAPtbIwpWRLKYcOxAB6QqKNCLx/UX/pWpGtyJmMQQBpxQgl3BT8daNp0h4Soc6VPZA9wEIQ5/a/8UpsBT9rwJGj5WdSBPSR8D54aULATPxsienQIDAQAB"
  }), FilterList({
    "EDEEE15A-6FA9-4FAC-8CA8-3565508EAAC3",
    "https://raw.githubusercontent.com/szpeter80/hufilter/master/hufilter.txt",
    "HUN: hufilter",
    {"hu"},
    "https://github.com/szpeter80/hufilter",
    "gemncmbgjgcjjepjkindgdhdilnaanlc",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4HNXDsDBPP4b/irxacZMYnaPjNMXS31e11nsFBvN9lFOkuwF3bEk9uEk0fDzocF6GSpXbUE0HVTqfKTTnZfvG9m+C3nT8j6N7BB/wST72s0zXCjSlLWJPGmFnFb/EDkFAGmA9FU4C+j28Obehd94OC9pSqu8DYK4LbMWPmk2fgpO9N3ZV/5Y2Ni69WKJwT72prSMzyVVEAYluCYPQWY93g6dJ9RBtwnHCmdK5TG/bN2q6f50Cw/aJSv8nshSdp+KJK6yi6fBOxF5Xb0Bj+xZGC4K4SW9JjElswaGJi2PX5I11w7xC24jNaW6BUHcJ6IXudIVmBFQxWWxkMVwfgqNlwIDAQAB"
  }), FilterList({
    "93123971-5AE6-47BA-93EA-BE1E4682E2B6",
    "https://raw.githubusercontent.com/heradhis/indonesianadblockrules/master/subscriptions/abpindo.txt", // NOLINT
    "IDN: ABPindo",
    {"id"},
    "https://github.com/heradhis/indonesianadblockrules",
    "egooomckhdgnfbpofhkbhbkiejaihdll",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAptA5jVa5JkYI2jt905om4OLSHGahwgS7tu7GG0sk1YNafOo4ajKrN96Kxj0fgwGrJPhU1UiTDmrgLTZSbuC3hAscbfhuakVNo1pyFfSAVoLWSrOq5l4k6zZK+y1ahxdyJvlbz06RWE6OhIqExxGqLyMjEknkPGxBVO0cKcYHiGYUxvVPxQOg+9fGieXMlSGs/L7Mty1oJOoZ4JcPIFeSvQ5ax48E7l+yAW6psNpPqRAZ5fm7hhZXjd5+3cfXXIMStgX3X0MUHjx2KpYlv3NxMjaZQOAZiuZ3W/H7VWnV7V/ScJ9Eb+e6iG4XS15f7vFQu4zPy4UTYOl6gXnIGWGmsQIDAQAB"
  }), FilterList({
    "4C07DB6B-6377-4347-836D-68702CF1494A",
    "https://secure.fanboy.co.nz/fanboy-indian.txt",
    "IN: Fanboy's India Filters",
    {"hi"},
    "https://www.fanboy.co.nz/filters.html",
    "fijddbnggnpidebfbejillgbopcikfpi",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAraHfJfoceCra3QEYhK2Njl8oUvtDOiT60o/HjpSsarfEgIgCKgqF/sUGK2PVL4c2CsF+fG+YzI4eGrJ85rXL5Z47lmP666r9lZcMVoGs2L7RT8UvsVwpyxiH868kLVxeM0NBas/jIy7jAul5/fGf0fRUF0TbSIG/tghO8nR4bloYEbSri9BWcl6ccMfAKsIalNWnP3uifjC/t7BWIkY486zq/UPXdIuJbRPWqJDTS6EZH5ZXFDkLE8tta3OrRV7n3ei2HdyTnJOVj2DSskKWwAA7fpYGVnmJouBqg++kC6YqSnaW2ElVzQIaiRigwXj4keAZzMoDbTrGh7g+LSVngwIDAQAB"
  }), FilterList({
    "C3C2F394-D7BB-4BC2-9793-E0F13B2B5971",
    "https://raw.githubusercontent.com/farrokhi/adblock-iran/master/filter.txt",
    "IRN: AdBlock Iran Filter",
    {"fa"},
    "https://github.com/farrokhi/adblock-iran",
    "dbcccdegkijbppmeaihneimbghfghkdl",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAm3AZE2ne7R55X2j6RxAQHAZKl1hNPgwLOFsYpfAJ6m0uXmKJspguWxatJ9jDBbYLmtXnwX2WORILq1+r4kFtTcN8GNYe7/7o5yDLucI/W9d2vCjmEg95v50MzVQZSwd2gNZVZtL1s0S6pBwX0zI+6kHIFr2xqGV/FNE8L75f30rriQ0xKmenI1OWjyn8gNqIp4mKZW6XxkMRRS9+e0ynDi4ysQA9Ub5YHJxm0t62eqTmIyemgRhP6Rdbi0+GXbqFPjDfC26rtD3wy5f3aYL1V+2ADpdDyCeNlwCH7+vC7LWujqNTgK8wVJ4eH5VbUKC1e9cm/T57OsHJMDC5fbUuswIDAQAB"
  }), FilterList({
    "48796273-E783-431E-B864-44D3DCEA66DC",
    "https://adblock.gardar.net/is.abp.txt",
    "ISL: Icelandic ABP List",
    {"is"},
    "https://adblock.gardar.net/",
    "njhlaafgablgnekjaodhgbaomabjibaf",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAqSwaNKhg90HCheaJu3sHocbZUjXDs90I0OmijNkDeS291wUjvXAm5YqhNE8aZmPSMVZBjBCwKXrrtTOkMA1b1uBqJ2P83fCZsgNZWbGTD8MorMrU6vyqkWCqLRc+bTTUgzAd55ckUJ/M+HVnjo6QfqUuB3kVzjpwJorQQZUYOLcgDY/Q5/tbrXI5+OGVxAb21pmnk8JHXNNWB2NvpA2o3p0ke/7WEoUH24l91ndOkXkN87eO8rSysl07Eq7gshbednYYiCxRPjuX0aPqbXMYNWXa5NdvIXFJcD2xV/l/QvXRYl+7Ca1igSXaiKc5eJyKSRqY4lf2vG0XCH6VZVxZuQIDAQAB"
  }), FilterList({
    "85F65E06-D7DA-4144-B6A5-E1AA965D1E47",
    "https://raw.githubusercontent.com/easylist/EasyListHebrew/master/EasyListHebrew.txt", // NOLINT
    "ISR: EasyList Hebrew",
    {"he"},
    "https://github.com/easylist/EasyListHebrew",
    "hjeidaaocognlgpdkfeenmiefipcffbo",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAoUnmZ/fnWAGhAywLBs5IX0OMxK6LOwtjljcwEkt8QD7ZKBekxq+MDrUuRPzav3ky9IyREhXe9F4UWBKPDD8ZQXZ57WQAAMAp3IxbgdAsTqqTEEReUVx+pzjl8lxdp7xEG2gpuM5wq7bjn4zJ3kcdj3vx7bec/YbYf4fV0brQPWghKf2sh3mHXOVh68wEFXYBvcWkGXfuBoRbB9WLflqZYRk3GrLllwBLn1Ag6iuKucvoyv7N23qXKIjqAhyKPmHx4l9w/v2c1pc3NB1af2xvtRWaQp19N98QouFFx5MwAI9+jR77Eox6QvRwA+L9CFkYlDTvT/aS3q+Zb1QH/8AE4QIDAQAB"
  }), FilterList({
    "A0E9F361-A01F-4C0E-A52D-2977A1AD4BFB",
    "https://raw.githubusercontent.com/gioxx/xfiles/master/filtri.txt",
    "ITA: ABP X Files",
    {},
    "https://xfiles.noads.it/",
    "agfanagdjcijocanbeednbhclejcjlfo",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAsorIQuFuMI5OaGYaYTu6+kZC4j3qPoWRoD7F9GS0IJC+VEk3XQ7UTsRXlrIxP9obmC7+pByP6hknBUzvKKS9I1v2voIqjUoydWOozbfoVoRhTLN3UiDnoDueXqXiv1MGLzY/ZcsxsxAlIiTcE7+/KdM6pJ72Mn/aLKU3escIJ5E5qOHJOFDLW9587JeWOzexaCOrtiZMclE0KWbUi7qB3Bz3auF6piSzoNGeI1NMwHSSAwhDOQ3UK09aqRKhyfBq6ugrrYyRAr3FWqmMBWkiTsr6SzrbQg3wcGbD+GDvoQmqVf8dH/WYG+srR6PyJdYH5mOQs6Yg+nu1gvwQ46Z74QIDAQAB"
  }), FilterList({
    "AB1A661D-E946-4F29-B47F-CA3885F6A9F7",
    "https://easylist-downloads.adblockplus.org/easylistitaly.txt",
    "ITA: EasyList Italy",
    {"it"},
    "https://forums.lanik.us/viewforum.php?f=96",
    "nkmllpnhpfieajahfpfmjneipnddhimi",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAurn1cJrIcCa8P7hjGex+OUHi19PRxmjJ5DuQlMAeIaKibwaQOZEPXSvD+O+xgxHZJs1o2DE8zfj6yrAmDfu9+/T0ArT2RWuopDMEfaKdeG0ylHP62WJC+KGUhCiTNmLyPxbU9AiwydVyFOam8vs4Tr+9I3lYKVeClQrtDRM34BTOAsuHRjiuIKoC0jDC2kc+BAsAbzhIdrkEDGD+qx0rCRnGL6c8xODe2PLKSkCSIsqOk44eYOkBqQd0SgmCvQjXS2XczMDNuV7DCZofErsy2iEv/2kzhkkN8GFwbRkYGN9LuK8rtekE34AvZKRHS6e/pHjUCYJb/2xv6elC+VLsJwIDAQAB"
  }), FilterList({
    "03F91310-9244-40FA-BCF6-DA31B832F34D",
    "https://raw.githubusercontent.com/k2jp/abp-japanese-filters/master/abpjf.txt", // NOLINT
    "JPN: ABP Japanese filters (日本用フィルタ)",
    {"ja"},
    "https://github.com/k2jp/abp-japanese-filters/wiki/Support_Policy",
    "ghnjmapememheddlfgmklijahiofgkea",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAsOoGwN4i751gHi1QmHMkFZCXFPseO/Q8qKOQViZI7p6THKqF1G3uHNxh8NjwKfsdcJLyZbnWx7BvDeyUw3K9hqWw4Iq6C0Ta1YEqEJFhcltV7J7aCMPJHdjZk5rpya9eXTWX1hfIYOvujPisKuwMNUmnlpaeWThihf4twu9BUn/X6+jcaqVaQ73q5TLS5vp13A9q2qSbEa79f/uUT8oKzN4S/GorQ6faS4bOl3iHuCT9abVXdy80WSut4bBERKgbc+0aJvi1dhpbCeM4DxVViM2ZccKvxSpyx4NvWj56dNKqFLvzoA4/Chz1udxifIXUHh0701s1Y4fLpY0wWP0uXQIDAQAB"
  }), FilterList({
    "51260D6E-28F8-4EEC-B76D-3046DADC27C9",
    "https://www.fanboy.co.nz/fanboy-korean.txt",
    "KOR: Fanboy's Korean",
    {},
    "https://forums.lanik.us/",
    "oidcknjcjepjgfpammgdalpnjefekhge",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAvCOljWKWLWfq/k/BE9gIZtI1MstmG+NcgGBGAP0R7xgaUMU5phdSbQf83Zt9ctwDRpisHWlGS6o+tk93zMIoJVj6RMQ2Zee6QPAKAGgwuCXF7A/ciI3lRyX7ts49XV8GAbasu1mBHntz+GpmOVmoiRxcDMUDDEqsSXgckCM9HkYvIyHQWyEgeulKdhQ2HoCptD2Wgmws6NzRTgQ94+DHu2o6J4MsG74h7L/cG3XB8WQNuqlpjjFIQTXftuUWDSkyR3tlmMxGN1PXAH6RZBNmwQTwdgrOAqEup82dWaO3BqoYGZdYeRaUGRc73iPdvvjZb1tvmqLdVSq7Ur1XJjJJTwIDAQAB"
  }), FilterList({
    "1E6CF01B-AFC4-47D2-AE59-3E32A1ED094F",
    "https://raw.githubusercontent.com/gfmaster/adblock-korea-contrib/master/filter.txt", // NOLINT
    "KOR: Korean Adblock List",
    {"ko"},
    "https://github.com/gfmaster/adblock-korea-contrib",
    "jboldinnegecjonfmaahihagfahjceoj",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEArcE90V2GoQ9InvoiVqxsEnhHhdJz68yyWA+HzQBxcxitqfF5Vu6gRiKB3+W/iTJ9XNE449krWrYAjDqVt6GF8ilpieIngwCNsl7jP5RRhrcrk65as4bawSimodit+TVi7ZpIFDWoalj5RIO+rjKfwkkBHujeM7qYr/Vm1NXre57ea80TvbdoA4XgnAYHKY5VV/WstFL8FR3Do+38EyaIzKfoiRrV97BoC440oifNdi0nJQhIgULfNjUolOh9eAQKuymId+WjZeSKSckUyKMQsQ0VVDjCbm5mwRqC5D+MT9L/8sKcdXBXuGrXyaZzp3eOmc41q01VkRLXCTrx37hGQwIDAQAB"
  }), FilterList({
    "45B3ED40-C607-454F-A623-195FDD084637",
    "https://raw.githubusercontent.com/yous/YousList/master/youslist.txt",
    "KOR: YousList",
    {"ko"},
    "https://github.com/yous/YousList",
    "djhjpnilfflibdflbkgapjfldapkjcgl",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAux80m8cYDEXwq+nMwmui6NCO9SFAdcGly5eq4uGEIQNB1R6Tr9JMqosHLZ4PnaUJqJwFfLWfmxzXj3q0DIpqqpdSq/jTYT/MvOldC+VQFO+NIjXhtysh4Z5F0BzlsQx/leMnV6yoyQjBX53n9cl3BvQK/EdbuQSDiNqX2TSVLm7hnr7Vf8m4XYRSCSJybY/1Tk3Cqgqywlkr+YN58L1/txXCQ9LJ5SxJ9I56TxqA1uT97hBmQikvnopuLh1SovDfjtCZwWwaGDD4ujW+Qaeh9dRrojS47iwG/Twu1xbb7ra8cn8BxdzsPjUSSurpPz/9sUooYOGJO44p7u77sxeTXQIDAQAB"
  }), FilterList({
    "4E8B1A63-DEBE-4B8B-AD78-3811C632B353",
    "https://raw.githubusercontent.com/EasyList-Lithuania/easylist_lithuania/master/easylistlithuania.txt",
    "LTU: Adblock Plus Lithuania",
    {"lt"},
    "https://margevicius.lt/easylist_lithuania/",
    "ekodlgldheejnlkhiceghfgdcplpeoek",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA5dB+7xR4lcPQCW84V4zhLiYhAvKxgdo2/cze+C8E3+ye1AO+a1CWbdPgft36vtTm4nkDzyC3P9O/aEU8jxShKEU1DDk8YBdRnvctQ9PPvwNyeS9LCYeT5a9crE9M/Z+kaFyq0SRe5cpowOBG8x4OYTt9Y7L9whEGzZYRZlgklli1AES6e2B9XUAdHXV/wHsaf2FrdPFtDfZZEFdr60edk4f0iGppiwkaGJiOWVF1ya47NoSMl4fIF7Klw9OkfKLJHjk9YXZmXCfqxQl8FnBFe/SzbSTVCAhdaggQAwG4VmojjMrBHcQl0VJDmpoY2jFZkiO3GLmAZCYIYaN1tFA8ZwIDAQAB"
  }), FilterList({
    "15B64333-BAF9-4B77-ADC8-935433CD6F4C",
    "https://notabug.org/latvian-list/adblock-latvian/raw/master/lists/latvian-list.txt", // NOLINT
    "LVA: Latvian List",
    {"lv"},
    "https://notabug.org/latvian-list/adblock-latvian",
    "hmabmnondepbfogenlfklniehjedmicd",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAst1posKDpKt3WLU07CziowQnBKYXzH2i/sDdJfMuTcKSNQvn9dxbHVLhg8Ih7NmN6SSJTRgb2PughdVNPXqlT3/jGioDC0gN8kBrBoN2YWgIW2wdvTCPvBOfwTOhGueQY6AtE7zD/3m9v6Wfcw07Rj84Su0qI1Zadmq2pBWo5z82vOAI2yV83YGDbnyK1JaFeLToYQmj+bMEojoZ4Lk4PbFmopVh1GkeOdCKtVN2NTIy43N/w0tS0wlLxjwTyZ6RIcK3VOhQXBqcpwKpKm/4WDksTvNRLZ8e526z/nqaasM/meS22hURh6NPtIOdy6/TspTzFPiRdj2xgNfQZ9oRxwIDAQAB"
  }), FilterList({
    "9D644676-4784-4982-B94D-C9AB19098D2A",
    "https://easylist-downloads.adblockplus.org/easylistdutch.txt",
    "NLD: EasyList Dutch",
    {"nl"},
    "https://forums.lanik.us/viewforum.php?f=100",
    "fbmjnabmpmfnfknjmbegjmjigmelggmf",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAqqfwmNS4XOq9pWC3XSMt5WcqoKaj3lRpYAwZKTP+6DwA9pG+Zw+0iWC95riSLjqPgX+0d2cZaqjinuNn3mUMOeGdbwSIeRLE50J5J/dMmkg5YO09orZKLBjMfJG5IDgfXdZLSJtmzKC4Xj2y6KSuQ7N0Sg5f1Ecc19nFbcFazCaIhKvcoA84J7Twf2IoCDuPMsGplgZCBtFQkKeqILaVhJZeD0my6pdC2KJREbM3eRnntE44O0sbmemCfHs9BV50hVb913zGDZ379eTqg3mPjvH+VnY+7RvjVPayJP4+51zRJYKi18W7KMry3sj4ZZ3EyNKmbwlGQOzAyd/Qtj4I3wIDAQAB"
  }), FilterList({
    "BF9234EB-4CB7-4CED-9FCB-F1FD31B0666C",
    "https://www.certyficate.it/adblock/adblock.txt",
    "POL: polskie filtry do Adblocka i uBlocka",
    {"pl"},
    "https://www.certyficate.it/adblock-ublock-polish-filters/",
    "paoecjnjjbclkgbempaeemcbeldldlbo",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAsUqWP6CeMx79UyZ3GZ1XcBGexIgml00sB286wZ7dJsfqG7oI0EGRoqrDeRreYcOTl+HvXsRJvR1FfkKJzD5svdhR4mn4lI+FXUDCvgEZ9CFa0YfASuoTIrdZtG74Twu2ai52ZJzrQ9ike97bdwzuZo+uymw26S+5/+IQbriIYoxEbJd7EryZuo+W65LdSat/NOKKf1QnVTIOoqMrXiewRYywnmZATfDIi0uKXuQfF15lbNBkQllmPH1xlMkz2WnvSvqI4HKPAmEFJWVUkiNhGKFZkTk1+88CgGGPVsKllxLaDOD+j8Kb0+h44RxObHTF/vFkfh8FfzujFj3HtevjCQIDAQAB"
  }), FilterList({
    "AD3E8454-F376-11E8-8EB2-F2801F1B9FD1",
    "https://raw.githubusercontent.com/tcptomato/ROad-Block/master/road-block-filters-light.txt",
    "ROU: Romanian Ad (ROad) Block List Light",
    {"ro"},
    "https://github.com/tcptomato/ROad-Block",
    "hojdjlebfkngledgkgecohjkjjojaekd",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAnhLXi2u795hnBUJi/vS8qtMGoTYk9NRefWk+SI6fkqVdiKs/eM6Y8v3To4HgNmtYb4jAoYctcq3/CS3hzGCLEwQbDuL8Y8UP5B6PWgzuiRrAobRl1DtXO1+Q57VIrYTpJVLCqaTulclys7Fka/wD5o78Y0vAfSenBZTzRUXwTZd9Z7SRNwcJyccbi7zL8UDWnJMBnD/dnV7t2q41MHiCgdzimOSuoRZmTBrupVc0QYhqoxy6ePkHFDGL2U25omAZckkzpQbtvJEE2lmg7YqnaSvGDzsmqd+j7hVWjpm/ncArLOWBCbER3MdHwFeOI2rFJWcO7GY5etQsA5128FAv0wIDAQAB"
  }), FilterList({
    "1088D292-2369-4D40-9BDF-C7DC03C05966",
    "https://adguard.com/en/filter-rules.html?id=1",
    "RUS: Adguard Russian Filter",
    {},
    "https://forum.adguard.com/forumdisplay.php?69-%D0%A4%D0%B8%D0%BB%D1%8C%D1%82%D1%80%D1%8B-Adguard", // NOLINT
    "dmoefgliihlcfplldbllllbofegmojne",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4p+4n2wNFQCqQBBJDsvs+oqNYGzX3cbpY7fKbCjrRVE5esJK5HZJDoUUg43pPvKrCOIQ+lF+dXpBaCNnO4O/7JeFt2IFRJnKhE3ipIBAAbFymfo5T2uWFdyh6HcK0FNyJ/7FyHnANe7vYhXJS1Fqmh6jTYkAEIbrbmxtzrDMefx3XJcVhUV3XAPlP+K3MerxudIH++4fn3X0vKob5oQQQ9ZZ1PVcW6ZdZTQwQWtaVDb6prT+ULaphRRmnZpZuRXyHMv9KC8YP3K5ou+/Yd3uxxMwKmJXD67ZoNMtS/Dtr0btQsLxiEgox5Swd4iqyLM/SMxr3LqgUIlNwn7KRbMnZwIDAQAB"
  }), FilterList({
    "DABC6490-70E5-46DD-8BE2-358FB9A37C85",
    "https://easylist-downloads.adblockplus.org/bitblock.txt",
    "RUS: BitBlock List (Дополнительная подписка фильтров)",
    {},
    "https://forums.lanik.us/viewforum.php?f=102",
    "fmcofgdkijoanfaodpdfjipdgnjbiolk",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEApGBuuxC9pqx2s5hUd3K8zeFUtMFt9X+rSKR3elqWztjIQrbOdXSeezHvhAUdzgc+Wv79ZoRf4i4amYs3Mg3wg783BAqLvlu9r6FsUAbcgVQtt+MT3Z4ZepwvzWU0NjUd1q4O2pNEUsE8SPjmOeb3KHOF5WX7CA1uIHT5xGQsU5Uh3VTZC8FIOGjCskDAAnJGUeOowlMBGL2UvlNQLiqzPSvI9byjwxIMN5OfCmxXXr4R9m88oVK2D1gj7vfwBVJcRdV8ner4ZSuT68ncSyaQRtgI3/QyHc0J6giCRFmF0bHN/5kjFIWrHg5+uiBQN4Qt39TVCUU024Fi2RGInvTTdQIDAQAB"
  }), FilterList({
    "80470EEC-970F-4F2C-BF6B-4810520C72E6",
    "https://easylist-downloads.adblockplus.org/advblock.txt",
    "RUS: RU AdList (Дополнительная региональная подписка)",
    {"ru", "uk", "be"},
    "https://forums.lanik.us/viewforum.php?f=102",
    "enkheaiicpeffbfgjiklngbpkilnbkoi",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEArVVgKRE868yup0HfX4+HyZmJVIk33AKivwvRRfjHRxeC+lLnRjNiY0LKS/K65J6SNLgUsZGfT5u4h4F423O/pbZl6zdfs5kOyStlmLPXhFtF/bIXIsUtdJ0R3dEz+nSg0C2L/FnE5Qr8M4thdmq/DIP1C70mj8pCnX1939hXyR0ymQkYp573O+LJ0q1L41jBqHzNKWngfBc79I2Kbt1pLluBT2X7zZVbb+1ap3Ad/VMeFDB2yurRs88cYJZOal7mgTgI/Zkuzsh2Dnql5+UNOCHinYjcOvUifGgkdsJIJxL57PxRzbriLCNjShoOV3Fpc0XYL1KSWvIVuW0bYeLmrwIDAQAB"
  }), FilterList({
    "AE657374-1851-4DC4-892B-9212B13B15A7",
    "https://easylist-downloads.adblockplus.org/easylistspanish.txt",
    "SPA: EasyList Spanish",
    {"es"},
    "https://forums.lanik.us/viewforum.php?f=103",
    "pdecoifadfkklajdlmndjpkhabpklldh",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA2eGyWcTM6Cpmkw6CBBxQbJCgp3Q4jyh+JR/Aqq5G+OFzxFpwlqW0dH9kNuUs30iSt1tt1gMZGYnhPKiGhtX3nV1iYg2K8k82wNqA5+ODfHxnnVn536UoC7rmjXL+mhpymxgkjGCQ+1HVmnCcSC9mxTPy65ihor+YZcRRPo0IhjQTx3NgdpzkGYvpQVjwnw3a5FpRBCbbp3X2x3EGV3DcjvT6DvvxSU/mAUPlXISo9OFHYUpADilqAevXQIs49LSmefSDu4pezGyR/JoRLh7QR4N3fC17V2E0GazWxvn2U985hPE3tvFcH+LM3EypVRCl6E9AiUZCeumqMBffyXw1AwIDAQAB"
  }), FilterList({
    "1FEAF960-F377-11E8-8EB2-F2801F1B9FD1",
    "https://filters.adtidy.org/extension/ublock/filters/9.txt",
    "spa, por: Adguard Spanish/Portuguese",
    {"es", "pt"},
    "https://github.com/AdguardTeam/AdguardFilters#adguard-filters",
    "jpolmkeojnkicccihhepfbkhcbicimpa",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAs1E/bf3s1EQeJY10IT5/ZMCzfMAm6SKyWUCeHBkWZLcfwyYLJww84EC2jCLeYgwukOmZjwtnDrasVhUyKOif7dKIBEZizsvSldi8tzHqTbX3PFKsLhRXCETbU0kkOlArGRGLaBIhgT07qPtlehYCZoDdowk42025fVtfVEMtZg8yBIqtFT/bDL96lRDQIW+1uAM3uFkzvRtQgsYhoI6JlyqFw6fqowRx8a+zHvtQzAUyIaTGf0OFEHwGCFlHXmTYpcOXlcUAXn4RnJvx+thpeDBtAvT6LubTLNQClBbwjGL5d7NlGNPByYdcZZcvGPmBWX/vnobY5QGP4lWxZvWfFwIDAQAB"
  }), FilterList({
    "418D293D-72A8-4A28-8718-A1EE40A45AAF",
    "https://raw.githubusercontent.com/betterwebleon/slovenian-list/master/filters.txt", // NOLINT
    "SVN: Slovenian List",
    {"sl"},
    "https://github.com/betterwebleon/slovenian-list",
    "lddghfaofadfpaajgncgkbjhalgohfkd",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4cRSF2Rg5SSG6mwE6NQCnX4a0MfzC9URqNFnI4Wf3d3a7CkhmVeNZHxSCGGLxNV9SjCi5tko7NdMqIwnN/vliZV+jnEDi8Lj+zz9nftkaGXe3jNoP7tr/+Qkqphc76j3wIpsQx/vBnfVTn5lrNynZL6qpFzX5dj4ukdJ6BOx1YTNdJV9LOyMWbC5rno1mpd14aS7R2T6xfnm3+nupaZMAbUeN/1bwxDdND/mbjFzFvkPCC+4m758tI/5kSJOefy8kNvp9BM64LXPA4sF59ttJtCIOJDAyhM1P0Danyze2g/0GGnojDuzZilfeSCeEpDsc+S78Tyqz/lMtxt2LZkvoQIDAQAB"
  }), FilterList({
    "7DC2AC80-5BBC-49B8-B473-A31A1145CAC1",
    "https://raw.githubusercontent.com/lassekongo83/Frellwits-filter-lists/master/Frellwits-Swedish-Filter.txt", // NOLINT
    "SWE: Frellwit's Filter List",
    {"sv"},
    "https://github.com/lassekongo83/Frellwits-filter-lists",
    "oimfmeehpinnecjghphifehbbnddjkmf",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA17Vf1qj8dWwYVtGpBHWc9gLiITU1XrTnb1sDASIeuKYp9JNBtEnBwy4oBlOoZd2uWFKxXrRtaimdwqa627gi9DB17t/RgzisXSpLubXbVVelRWllaX26SioGxsGcQhS2/e1Bc0inQ8GODM6mk5FPZ9RObFN1N/QVz35anN4VNcjtETD/XpujYXE1BU3C0KGBlWwc+cQZ6sGojWEPrb7aRXSTJ5y/ugwGomTTpbT+Jt9nFrMfuAmJHvWS0Ev96dDmn1zsuoPGUExVFjGBunphRYMVCg9LUGzY0FN5+dp6fljrTJrtUOEfvh40vmjahKd0w6bKpgTAOUEaWulmVSr37QIDAQAB"
  }), FilterList({
    "658F092A-F377-11E8-8EB2-F2801F1B9FD1",
    "https://raw.githubusercontent.com/easylist-thailand/easylist-thailand/master/subscription/easylist-thailand.txt",
    "THA: EasyList Thailand",
    {"th"},
    "https://github.com/easylist-thailand/easylist-thailand",
    "jplgiejfnpolnfnigblbfeeidoimingd",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAy4EQ0IdWDAAqRT6ymtxWG32Whfmv5TMxKuEP3rs9LMfa+iU+xiyE/XvWjgA58n4Bs2vQgefCoqY+a9B0TUkqz0nDcrBi371w1EZSxNRySslO5VvLHdRdYwynTMwDsAlHIPZ6pgh9zwprW1Lxz31CC2EHJeBGmBQ/S8My/VRiN8Y2Jj7yZDX1rTvBrYPj5XAwe2MAPAsMJD4lHcx7uClEbVq/4AxNpmNay5kamFaX6qt8/765RyPYuqgneharP7EJ9HToH56l/KR7doOywTyVPQYvEhD+a1mioMfEtYNxvqY4lKDhctTV8aU7RItAgwGTW+msldvdPfs3QWV5o7yrtQIDAQAB"
  }), FilterList({
    "1BE19EFD-9191-4560-878E-30ECA72B5B3C",
    "https://adguard.com/filter-rules.html?id=13",
    "TUR: Adguard Turkish Filter",
    {"tr"},
    "https://forum.adguard.com/forumdisplay.php?51-Filter-Rules",
    "oooemoeokehlgldpjjhcgbndjcekllim",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA2We4hmp3TwsrKyOb6rF/mCjy9TW3w9n9CD1rZMXUF3U6CCgxH4lps5HiLlxUFaIhhcUEXrGlXbk4TE2LlTv4VS53O23YixZXQ/xMmpWSyBvc3/jBCrAAcvDLAZY53J1T/9t7DNZdpXkX3rNpYB4L5/5dyzQI+sZZoTBe5dLyJOR1uDZJphpXRWSKqBRLn4SJ5uOGgtqG5J4rMhB+SUrNhWs8AyM8+tdoaxOjx7n+PA2Rx7/foty1Bbd7Hfc1Eg0C9R40inJNgH+IDxZ07ZFqiAuY1Z16lr4bwunk7ft4tTafci0M2t86JkoH0B4yiTBKthB6AkmZ0/dejeQeOBszYQIDAQAB"
  }), FilterList({
    "6A0209AC-9869-4FD6-A9DF-039B4200D52C",
    "https://www.fanboy.co.nz/fanboy-vietnam.txt",
    "VIE: Fanboy's Vietnamese",
    {"vi"},
    "https://forums.lanik.us/",
    "cklgijeopkpaadeipkhdaodemoenlene",
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAymFhKEG/UJ8ZyKjdx4xfRFtECXdWXixG8GoS3mrw/haeVQoB1jXmPBQTZfL2WGZqYvrAkHRRel7XEoZNYziP3bCYbS4yVqKnDUp1u5GIsMsN0Pff1O1SHEbqClb79vAVhftNq1VQkHPpXQdoSiINQ12Om8WbOIuaNxkrTToFW7XRMtbI3tluoLUSy9YTkCEGah68Dl1uL6nOzOxaMV1iQRRk5Pw4ugTzwGHHL2U2kDYDNrlywK8cUIFgtZskqQ/TF1zF6u9xTGjwjB9X319XrTg2llcojCgj/dllBuXL2aJoDsS3qAVzqbSYxIE6bQU8JX8wv+KCDMpJt/dHPQqOMwIDAQAB"
  })
};

#endif  // LISTS_REGIONS_H_
